import argparse
import ctypes
import binascii
import sys

# --- E32 Constants (from C++ Header) ---

KImageImpFmtMask = 0xf0000000
KImageImpFmtShift = 28
KImageImpFmt_PE = 0
KImageImpFmt_ELF = 1 # Note: In flags this is 1, not 2. 0x10000000 >> 28 = 1

KImageHdrFmtMask = 0x0f000000
KImageHdrFmtShift = 24
KImageHdrFmt_V = 2

KImageABIMask = 0x00000018
KImageABIShift = 3
KImageABI_EABI = 1

# --- E32 Structures (from C++ Header) ---

class SSecurityInfo(ctypes.LittleEndianStructure):
    _fields_ = [
        ('iSecureId', ctypes.c_uint32),
        ('iVendorId', ctypes.c_uint32),
        ('iCaps1', ctypes.c_uint32),
        ('iCaps2', ctypes.c_uint32),
    ]

class E32ImageHeaderV(ctypes.LittleEndianStructure):
    """
    A complete ctypes mapping of the E32ImageHeaderV class, based on the
    provided C++ header file for maximum accuracy with modern EKA2 binaries.
    """
    _fields_ = [
        # E32ImageHeader base
        ('iUid1', ctypes.c_uint32),
        ('iUid2', ctypes.c_uint32),
        ('iUid3', ctypes.c_uint32),
        ('iUidChecksum', ctypes.c_uint32),
        ('iSignature', ctypes.c_uint32),
        ('iHeaderCrc', ctypes.c_uint32),
        ('iModuleVersion', ctypes.c_uint32),
        ('iCompressionType', ctypes.c_uint32),
        ('iToolsVersion_major', ctypes.c_uint8), # TVersion is split
        ('iToolsVersion_minor', ctypes.c_uint8),
        ('iToolsVersion_build', ctypes.c_uint16),
        ('iTimeLo', ctypes.c_uint32),
        ('iTimeHi', ctypes.c_uint32),
        ('iFlags', ctypes.c_uint32),
        ('iCodeSize', ctypes.c_int32),
        ('iDataSize', ctypes.c_int32),
        ('iHeapSizeMin', ctypes.c_int32),
        ('iHeapSizeMax', ctypes.c_int32),
        ('iStackSize', ctypes.c_int32),
        ('iBssSize', ctypes.c_int32),
        ('iEntryPoint', ctypes.c_uint32),
        ('iCodeBase', ctypes.c_uint32),
        ('iDataBase', ctypes.c_uint32),
        ('iDllRefTableCount', ctypes.c_int32),
        ('iExportDirOffset', ctypes.c_uint32),
        ('iExportDirCount', ctypes.c_int32),
        ('iTextSize', ctypes.c_int32),
        ('iCodeOffset', ctypes.c_uint32),
        ('iDataOffset', ctypes.c_uint32),
        ('iImportOffset', ctypes.c_uint32),
        ('iCodeRelocOffset', ctypes.c_uint32),
        ('iDataRelocOffset', ctypes.c_uint32),
        ('iProcessPriority', ctypes.c_uint16),
        ('iCpuIdentifier', ctypes.c_uint16),
        # E32ImageHeaderComp extension
        ('iUncompressedSize', ctypes.c_uint32),
        # E32ImageHeaderV extension
        ('iS', SSecurityInfo),
        ('iExceptionDescriptor', ctypes.c_uint32),
        ('iSpare2', ctypes.c_uint32),
        ('iExportDescSize', ctypes.c_uint16),
        ('iExportDescType', ctypes.c_uint8),
        # iExportDesc is a variable array, so we stop here.
    ]

class E32Patcher:
    def __init__(self, infile, outfile):
        self.infile = infile
        self.outfile = outfile
        
        try:
            with open(self.infile, 'rb') as f:
                self.data = bytearray(f.read())
            print(f"Successfully read {len(self.data)} bytes from '{self.infile}'")
        except FileNotFoundError:
            print(f"Error: Input file '{self.infile}' not found.")
            sys.exit(1)
            
        if len(self.data) < ctypes.sizeof(E32ImageHeaderV):
            print("Error: File is smaller than a valid E32 header.")
            sys.exit(1)
            
        self.header = E32ImageHeaderV.from_buffer(self.data)

    def analyze_flags(self):
        """Prints a human-readable analysis of the header flags."""
        print("\n--- E32 Header Analysis ---")
        
        imp_fmt_val = (self.header.iFlags & KImageImpFmtMask) >> KImageImpFmtShift
        imp_fmt_str = {0: "PE", 1: "ELF", 2: "PE2"}.get(imp_fmt_val, "Unknown")
        print(f"Import Format      : {imp_fmt_str} (Value: {imp_fmt_val})")

        hdr_fmt_val = (self.header.iFlags & KImageHdrFmtMask) >> KImageHdrFmtShift
        hdr_fmt_str = {2: "V Format"}.get(hdr_fmt_val, "Original/J")
        print(f"Header Format      : {hdr_fmt_str} (Value: {hdr_fmt_val})")

        abi_val = (self.header.iFlags & KImageABIMask) >> KImageABIShift
        abi_str = {1: "EABI"}.get(abi_val, "GCC98r2")
        print(f"ABI                : {abi_str} (Value: {abi_val})")
        
        print(f"Import Offset      : {hex(self.header.iImportOffset)}")
        print(f"DLL Reference Count: {self.header.iDllRefTableCount}")
        print(f"Export Dir Offset  : {hex(self.header.iExportDirOffset)}")
        print(f"Export Dir Count   : {self.header.iExportDirCount}")
        print("---------------------------\n")

    def patch(self):
        if self.header.iSignature != 0x434f5045: # 'EPOC' in little-endian
            print("Error: This does not appear to be a valid E32 file (bad signature).")
            return False

        self.analyze_flags()

        if self.header.iImportOffset == 0 or self.header.iDllRefTableCount == 0:
            print("Error: Binary has no imports to patch according to its header.")
            return False

        print(f"Found {self.header.iDllRefTableCount} imported DLL(s). Searching for drtaeabi...")
        
        import_format = (self.header.iFlags & KImageImpFmtMask) >> KImageImpFmtShift
        import_section_start = self.header.iImportOffset
        import_block_offset = import_section_start + 4 # Skip section size
        
        patch_applied = False

        for _ in range(self.header.iDllRefTableCount):
            offset_of_dll_name = int.from_bytes(self.data[import_block_offset:import_block_offset+4], 'little')
            num_imports = int.from_bytes(self.data[import_block_offset+4:import_block_offset+8], 'little')

            dll_name_start = import_section_start + offset_of_dll_name
            dll_name_end = self.data.find(b'\x00', dll_name_start)
            current_dll_name = self.data[dll_name_start:dll_name_end].decode('ascii', errors='ignore').lower()

            if current_dll_name == "drtaeabi{000a0000}.dll":
                print(f"Found target DLL: {current_dll_name} with {num_imports} imports.")
                imports_list_offset = import_block_offset + 8
                
                for i in range(num_imports):
                    if import_format == KImageImpFmt_ELF:
                        reloc_offset_in_code = int.from_bytes(self.data[imports_list_offset+(i*4):imports_list_offset+(i*4)+4], 'little')
                        ordinal_location = self.header.iCodeOffset + reloc_offset_in_code
                        
                        if ordinal_location + 4 > len(self.data):
                            print(f"  > Error: Calculated ordinal location {hex(ordinal_location)} is out of bounds.")
                            continue

                        packed_ord = int.from_bytes(self.data[ordinal_location:ordinal_location+4], 'little')
                        current_ord = packed_ord & 0xFFFF
                        
                        if current_ord == 222:
                            new_packed_ord = (packed_ord & 0xFFFF0000) | 135
                            self.data[ordinal_location:ordinal_location+4] = new_packed_ord.to_bytes(4, 'little')
                            patch_applied = True
                            continue
                        if current_ord == 223:
                            new_packed_ord = (packed_ord & 0xFFFF0000) | 155
                            self.data[ordinal_location:ordinal_location+4] = new_packed_ord.to_bytes(4, 'little')
                            patch_applied = True
                    else: # PE Style
                        print("Error: This script's ELF logic is required, but the binary is not ELF format. Aborting.")
                        return False
            
            if patch_applied: break
            import_block_offset += 8 + (num_imports * 4)

        self.save()
        return True
        
    def save(self):
        try:
            with open(self.outfile, 'wb') as f:
                f.write(self.data)
            print(f"\nSuccessfully saved patched binary to '{self.outfile}'")
        except IOError as e:
            print(f"Error: Could not write to output file '{self.outfile}'. {e}")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="A tool to patch an import ordinal in a Symbian E32 binary (EKA2 compatible).",
        epilog="Example: python e32_patcher.py myapp.dll myapp_patched.dll"
    )
    parser.add_argument("infile", help="Path to the input E32 binary file.")
    parser.add_argument("outfile", help="Path to write the patched binary file.")
    
    args = parser.parse_args()
    patcher = E32Patcher(args.infile, args.outfile)
    patcher.patch()