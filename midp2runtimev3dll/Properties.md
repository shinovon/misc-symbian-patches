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

## Helpers

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


## Java applications

### `cc.nnproject.symbianext.getMidletSuiteVersion:<name>,<vendor>`

Gets version of installed Java application

Parameters:
- name: MIDlet suite name
- vendor: MIDlet suite vendor

Returns version in `<major>.<minor>.<build>` format if MIDlet suite is found, null otherwise

Example:
```
if (System.getProperty("cc.nnproject.symbianext.getMidletSuiteVersion:MPGram,nnproject") == null) {
    // not installed...
}
```

---

### `cc.nnproject.symbianext.launchMidlet:<name>,<vendor>[,<num>[,<cmd>]]`

Launches Java application

Parameters:
- name: MIDlet suite name
- vendor: MIDlet suite vendor
- num: MIDlet number starting from 1, defaults to 1 if not specified
- cmd: Command line arguments, optional

Returns 0 if operation was successful, otherwise numeric error code

Example:
```
if ("0".equals(System.getProperty("cc.nnproject.symbianext.launchMidlet:MPGram,nnproject,1,-Dasd=dsa -Ddsa=asd"))) {
    // ...
}
```

---

### `cc.nnproject.symbianext.getMidletUid:<name>,<vendor>[,<num>]`

Gets registered UID of Java application, can be used for isAppRunning, bringAppToForeground

Parameters:
- name: MIDlet suite name
- vendor: MIDlet suite vendor
- num: MIDlet number starting from 1, defaults to 1 if not specified

Returns application UID

Example:
```
String mpgramUid = System.getProperty("cc.nnproject.symbianext.getMidletUid:GH2ME,nnproject");
```

---



## Symbian applications

### `cc.nnproject.symbianext.isAppInstalled:<uid>`

Checks if Symbian application is installed

Parameters:
- uid: Application UID (decimal, or hexadecimal with 0x)

Returns true if application can be run, false otherwise

Example:
```
// Is ClassiCube installed
if ("true".equals(System.getProperty("cc.nnproject.symbianext.isAppInstalled:0xe212a5c2"))) {
    // ...
}
```

---

### `cc.nnproject.symbianext.isAppRunning:<uid>`

Checks if application is running

Parameters:
- uid: Application UID (decimal, or hexadecimal with 0x)

Returns true if application is running, false otherwise

Example:
```
// Is X-Plore running
if ("true".equals(System.getProperty("cc.nnproject.symbianext.isAppRunning:0xa0000bcd"))) {
    // ...
}
```

---

### `cc.nnproject.symbianext.launchApp:<uid>[,<document>]`

Launches Symbian application by UID

Parameters:
- uid: Application UID (decimal, or hexadecimal with 0x)
- document: ApaCommandLine document path, optional

Returns 0 if operation was successful, otherwise numeric error code

Example:
```
// Launch CorePlayer
if ("0".equals(System.getProperty("cc.nnproject.symbianext.launchApp:0x20012fa6"))) {
    // ...
}
```

---

### `cc.nnproject.symbianext.bringAppToForeground:<uid>[,<message>,<data>]`

Brings application to foreground if it is running, optionally sends a message.

FUTURE

Parameters:
- uid: Application UID (decimal, or hexadecimal with 0x)
- message: Message UID (decimal), optional
- data: Message parameters (non-Unicode), optional

Returns true if operation was successful, false otherwise

Example:
```
String browserUid = "0x10008d39";
if ("true".equals(System.getProperty("cc.nnproject.symbianext.isAppRunning:" + browserUid))) {
    if ("true".equals(System.getProperty("cc.nnproject.symbianext.bringAppToForeground:" + browserUid + ",0,http://nnproject.cc/"))) {
        // ...
    }
} else if ("0".equals(System.getProperty("cc.nnproject.symbianext.launchApp:" + browserUid + ",http://nnproject.cc/"))) {
    // ...
}
```

---



## SIS registry

### `cc.nnproject.symbianext.getInstalledSisVersion:<uid>`

Checks if SIS package is installed, only for EKA2

Parameters:
- uid: SIS package UID (decimal, or hexadecimal with 0x)

Returns version in `<major>.<minor>.<build>` format if SIS package is installed, null otherwise

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



## Nokia extensions

### `com.nokia.memoryramfree`

Returns amount of free RAM in bytes, mimics S60v3 FP2 behavior

Check Java Developers Library for documentation

---

### `com.nokia.mid.batterylevel`

Returns battery charge level 0-100, mimics S60v3 FP2 behavior

Check Java Developers Library for documentation


