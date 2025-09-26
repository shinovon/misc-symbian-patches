import argparse
import sys

def process_file(input_path, output_path):
    key = bytearray.fromhex(
        "10 1F 8B 19 63 80 F4 12 27 E5 ED 61 34 A6 D2 05"
    )
    key_length = len(key)

    try:
        print(f"Reading file: '{input_path}'...")
        with open(input_path, 'rb') as f_in:
            data = f_in.read()
            print(f"Successfully read {len(data)} bytes.")
    except FileNotFoundError:
        print(f"Error: The input file '{input_path}' was not found.", file=sys.stderr)
        sys.exit(1)
    except IOError as e:
        print(f"Error reading from '{input_path}': {e}", file=sys.stderr)
        sys.exit(1)

    # Apply the XOR transformation byte by byte.
    processed_data = bytearray(len(data))
    for i in range(len(data)):
        processed_data[i] = data[i] ^ key[i % key_length]

    try:
        print(f"Writing file: '{output_path}'...")
        with open(output_path, 'wb') as f_out:
            f_out.write(processed_data)
        print(f"Successfully wrote {len(processed_data)} bytes.")
    except IOError as e:
        print(f"Error writing to '{output_path}': {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    # Set up the command-line argument parser for a user-friendly interface.
    parser = argparse.ArgumentParser(
        description="Encrypt or Decrypt DLS soundbank files for a specific synthesizer.",
        epilog="Example usage:\n"
               "  To decrypt: python %(prog)s decrypt ensb.dat decoded.dls\n"
               "  To encrypt: python %(prog)s encrypt my_soundbank.dls ensb.dat",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    # Define the required command-line arguments.
    parser.add_argument(
        'mode',
        choices=['encrypt', 'decrypt'],
        help="The operation to perform."
    )
    parser.add_argument(
        'input_file',
        help="The path to the source file to process."
    )
    parser.add_argument(
        'output_file',
        help="The path where the resulting file will be saved."
    )

    # Parse the arguments provided by the user from the command line.
    args = parser.parse_args()

    # Run the main processing function with the specified files.
    process_file(args.input_file, args.output_file)
    
    print(f"\nOperation '{args.mode}' completed successfully.")