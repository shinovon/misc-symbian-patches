# Custom properties

### `cc.nnproject.symbianext.version`

Returns version of midp2runtimev2.dll extension

Example:
```
if (System.getProperty("cc.nnproject.symbianext.version") != null) {
    // patch installed
}
```

---

### `cc.nnproject.symbianext.checkTLS12`

Checks if TLS 1.2 patch is available

Returns `mbedtls`, `bearssl`, or `false`

Example:
```
String s = System.getProperty("cc.nnproject.symbianext.checkTLS12");
if (s != null && !s.equals("false")) {
    // go proxy-less...
}
```

---

### `cc.nnproject.symbianext.isAppInstalled:<uid>`

Checks if Symbian application is installed

uid: Application UID (decimal, or hexadecimal with 0x)

Returns true if application is runnable, false otherwise

Example:
```
// Is ClassiCube installed
if ("true".equals(System.getProperty("cc.nnproject.symbianext.isAppInstalled:e212a5c2"))) {
    // ...
}
```

---

### `cc.nnproject.symbianext.launchApp:<uid>[,<document>]`

Launches Symbian application by UID

Parameters:
- uid: Application UID (decimal, or hexadecimal with 0x)
- document: ApaCommandLine document path, optional

Returns true if launch was successful, false if application was not found

Example:
```
// Launch S60 browser
if ("true".equals(System.getProperty("cc.nnproject.symbianext.launchApp:10008d39"))) {
    // ...
}
```

---

### `cc.nnproject.symbianext.getInstalledSisVersion:<uid>`

Checks if SIS package is installed, only for EKA2

Parameters:
- uid: SIS package UID (decimal, or hexadecimal with 0x)

Returns version in <major>.<minor>.<build> format if SIS package is installed, null otherwise

Example:
```
// MBedTLS library
String version = System.getProperty("cc.nnproject.symbianext.getInstalledSisVersion:20292b6b");
if (version == null) {
    // not installed...
} else if (parseVersion(version)[1] < 4) {
    // ask user to update...
}
```

---

### `cc.nnproject.symbianext.getMidletSuiteVersion:<name>,<vendor>`

Gets version of installed Java application

Parameters:
- name: MIDlet suite name
- vendor: MIDlet suite vendor

Returns version in <major>.<minor>.<build> format if MIDlet suite is found, null otherwise

Example:
```
if (System.getProperty("cc.nnproject.symbianext.getMidletSuiteVersion:MPGram,nnproject") == null) {
    // not installed...
}
```

---

### `cc.nnproject.symbianext.launchMidlet:<name>,<vendor>,<num>,[<midlet>[,<cmd>]]`

Launches Java application

Parameters:
- name: MIDlet suite name
- vendor: MIDlet suite vendor
- num: MIDlet number starting from 1, optional
- cmd: Command line arguments, optional

Returns true if launch was successful, false otherwise

Example:
```
// Launch MPGram
if ("true".equals(System.getProperty("cc.nnproject.symbianext.launchMidlet:MPGram,nnproject,1,-Dasd=dsa"))) {
    // ...
}
```

---

### `com.nokia.memoryramfree`

Returns amount of free RAM in bytes, mimics S60v3 FP2 behavior

Check Java Developers Library for documentation

---

### `com.nokia.mid.batterylevel`

Returns battery charge level 0-100, mimics S60v3 FP2 behavior

Check Java Developers Library for documentation


