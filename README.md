# FlashMemEditorUnix
Proof of concept of ptrace and proc/pid/mem for editing adobe flash scripts on the air.

## Proof of concept of ptrace function in C
With this code you can edit easily flash games memory easily (I know that I'm living in the past)

## Usage
```
git clone git@github.com:JoseCarlosGarcia95/FlashMemEditorUnix.git'
cd FlashMemEditorUnix

```

Edit main.c with your payloads.

```
make
sudo ./flashhack
```

## Functions
```C
/**
    Search a string and then replace it.
*/
void flash_memory_payload_string(char *, char *);

/**
    Search a integer value and then replace it.
*/
void flash_memory_payload_int(int, int);

/**
    Wait until you open a flash executable.
*/
void flash_memory_wait_until_flash();
```
