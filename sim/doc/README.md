# sim_flash_mem

## Introduction
simulated flash memory class.  

## Build and test
need cmake installed.  
- `make test`  

without cmake:  
- `bash tests/test_sim_flash.sh`

## Classes with namespace sim

### SimStatusRegister: 
status register read/write  
-  shared_mutex protected for multi-thread access

### SimFlashMem: 
root class. pure virtual class.  
- Flash Specific operations:
    - `setUID()`
    - `getInfo()`
    - `getState()`
- hardware related operations:
    - `HardwareCheck()`
    - `setWP()`
    - `setClock()`
    - `setVccc()`
    - `setVpp()`   `TODO`
- Backdoor operations: virtual functions
    - Write/Read/Erase/BlankCheck
- `Transfer()`: per-byte and cs-rising handler functions
- Misc features:
    - `isStatusWriteDisabled()`
    - `isWriteEnabled()`
    - `isWriteInProgress()`
    - `getDefaultPattern()`
    - `getBlankPattern()`
    - `getReleaseDeepPowerDownCmd()`
    - `addMessage()`
    - `clearMessage()`
    - `getMessage()`
    - `enableDump()`
    - `Dump()`
    - `getPageIndex()`
    - `getSectorIndex()`
    - `AddressConvert()`

### SimFlashMem<typename SR, uint8_t kBlankValue, size_t kPageSize>:
Status Register (SR) and Blank Value (kBlankValue) and Page Size (kPageSize) are template parameters.  
- Implemented functions:  
    - `BackdoorWrite()`
    - `BackdoorRead()`
    - `BackdoorErase()`
    - `BackdoorBlankCheck()`
    - `getPage()`
- Architecture:  
`using page_mem_t = std::array<uint8_t, kPageSize>;`  
`using sector_mem_t = std::map<uint32_t, page_mem_t>;`  
`using chip_mem_t = std::map<uint32_t, sector_mem_t>;`  
data structure design according to HW-spec. and get higher performance.  
foud-tier architecture: chip -> sector -> page -> byte.  
ChipErase: `mem_.clear()`  
SectorErase: `mem_.erase(sector_index)`  
PageProgram: `mem_[sector_index][page_index] = page_mem__t(page_data)`  
RandomProgram: `mem_[sector_index][page_index][byte_index] = byte_data`  

### implemented flash classes:
#### SimM25Pxx
FlashClass = M25Pxx in database.  
- override functions:
    - `setWP()`
    binding to SR::SRWD(BIT[7])  
    - `isStatusWriteDisabled()`
    binding to SR::SRWD(BIT[7])  
    - `isWriteEnabled()`
    binding to SR::WEL(BIT[1]) and SR::Protect(BIT[2:3])
    - `isWriteInProgress()`
    binding to SR::WIP(BIT[0])
    - `PerByteHandle()`
    - `CsRisingHandle()`
    - `asyncChipErase()`
    multi-thread chip-erase at background. `CONFIG_CHIP_ERASE_MS` could simulate the time.
    - `asyncSectorErase()`
    multi-thread sector-erase at background. `CONFIG_SECTOR_ERASE_MS` could simulate the time.
    - `asyncPageProgram()`
    multi-thread page-program at background. `CONFIG_PAGE_PROGRAM_MS` could simulate the time.

## TODO list
- [x] unit test with framework
- [ ] more flash class
- [ ] Vpp support