# USB Command List
## usbdriver.c
- AssignSF600orSF700var and QueryBoard  
`InCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x8, 0, 0, buf, 32);`  
- dediprog_start_appli  
`InCtrlRequest(URB_FUNCTION_VENDOR_OTHER, 0xb, 0, 0, buf, 1);`  
- dediprog_get_chipid  (new)
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x1, 0xff, 0, {0x9F}, 1);`  
`InCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x1, 0, 0, buf, 3);`  
- dediprog_get_chipid  (old)
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x1, 0xff, 1, {0x9F}, 1);`  
`InCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x1, 0, 0, buf, 3);`  
- dediprog_set_spi_voltage  (new)
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x9, volt, 0, nullptr, 0);`  
  if sendFFSequence `Sleep(200);FlashCommand_SendCommand_OutOnlyInstruction({0xFF, 0xFF, 0xFF, 0xFF}, 4);`  
- dediprog_set_spi_voltage  (old)
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x9, volt, (0x4|g_CurrentSeriase), nullptr, 0);`  
  if sendFFSequence `Sleep(200);FlashCommand_SendCommand_OutOnlyInstruction({0xFF, 0xFF, 0xFF, 0xFF}, 4);`  
- dediprog_set_vpp_voltage and SetVppVoltage  
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x3, vpp_selector, 0, nullptr, 0);`   
- dediprog_set_spi_clk and SetSPIClockValue  
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x61, hz_selector, 0, nullptr, 0);`  
- flash_ReadId  (new)
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x1, 1, 0, {rad_id_code}, 1);`  
`InCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x1, 1, 0, buf, out_data_size);`  
- flash_ReadId  (old)
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x1, 0, 1, {rad_id_code}, 1);`  
`InCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x1, TIMEOUT, 0, buf, out_data_size);`  
## board.c
- ReadSF700AndSF600PG2SN  
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x71, 0, 0, {0, 0, 0, 2, 0, 0}, 6);`   
`BulkPipeRead(buf, 512);`   
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x71, 0, 0, {0, 0, 0, 2, 0, 0}, 6);`   
`BulkPipeRead(buf, 512);`   
- ReadOnBoardFlash  (new)  
`InCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x5, 0, ReadUID, buf, 16);`  
- ReadOnBoardFlash  (old)  
`InCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0, 0x5, ReadUID, buf, 16);`  
- SetIO (new)  
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x7, (ioState & 0x0F) | 0x70, 0, nullptr, 0);`  
- SetIO (old)  
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x7, ioState, 7, nullptr, 0);`  
- SetTargetFlash  
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x4, StartupMode, 0, nullptr, 0);`  
- SetLEDProgBoard (new)  
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x7, Color | (g_IO1Select << 1), 0, nullptr, 0);`  
- SetLEDProgBoard (0ld)  
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x9, Color >> 8, 0, nullptr, 0);`  
- LeaveSF600Standalone  
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0xA, Enable, 0, nullptr, 0);Sleep(100);`  
- SetCS  
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x14, value, 0, nullptr, 0);`  
- SetIOModeToSF600 or SetIOMOdeValue  
`OutCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x15, value, 0, nullptr, 0);`  
- GetFPGAVersion  
`InCtrlRequest(URB_FUNCTION_VENDOR_ENDPOINT, 0x1C, 0, 0, buf, 2);`  
- ReadUID
`InCtrlRequest(URB_FUNCTION_VENDOR_OTHER, 7, 0, 0xEF00, buf, 3);`  
- SetVpp4IAP
`InCtrlRequest(URB_FUNCTION_VENDOR_OTHER, bOn?0:1, 0, 0, buf, 1);Sleep(200);`  
- UnlockRASS
`InCtrlRequest(URB_FUNCTION_VENDOR_OTHER, 3, 0, 0, buf, 1);`  
- EraseST7Sectors
`InCtrlRequest(URB_FUNCTION_VENDOR_OTHER, bSect1 ? 0x04 : 0x05, 0, 0, buf, 1);`  
- ProgramSectors
`OutCtrlRequest(URB_FUNCTION_VENDOR_OTHER, 0x1, 0, 0, buf, 256);`  
`InCtrlRequest(URB_FUNCTION_VENDOR_OTHER, 8, 0, iStartAddr&0xFFFF, buf, 1);`  
## SerialFlash.c
int SerialFlash_doWRSR(unsigned char cSR, int Index)
int SerialFlash_doRDSR(unsigned char* cSR, int Index)
void SerialFlash_waitForWEL(int Index)
bool SerialFlash_waitForWIP(int Index)
int SerialFlash_doWREN(int Index)
int SerialFlash_doWRDI(int Index)
int SerialFlash_protectBlock(int bProtect, int Index)
int SerialFlash_EnableQuadIO(int bEnable, int boRW, int Index)
int SerialFlash_Enable4ByteAddrMode(int bEnable, int Index)
int SerialFlash_rangeBlankCheck(struct CAddressRange* Range, int Index)
int SerialFlash_rangeProgram(struct CAddressRange* AddrRange, unsigned char* vData, int Index)
int SerialFlash_rangeRead(struct CAddressRange* AddrRange, unsigned char* vData, int Index)
int SerialFlash_DoPolling(int Index)
int SerialFlash_is_good()
int SerialFlash_batchErase_W25Mxx_Large(uintptr_t* vAddrs, size_t AddrSize, int Index)
int SerialFlash_batchErase(uintptr_t* vAddrs, size_t AddrSize, int Index)
int SerialFlash_rangeErase(unsigned char cmd, size_t sectionSize, struct CAddressRange* AddrRange, int Index)
bool SerialFlash_chipErase(int Index)
int SerialFlash_DieErase(int Index)
int SerialFlash_bulkPipeProgram(struct CAddressRange* AddrRange, unsigned char* vData, unsigned char modeWrite, unsigned char WriteCom, int Index)
int SerialFlash_bulkPipeProgram_Micron_4Die(struct CAddressRange* AddrRange, unsigned char* vData, unsigned char modeWrite, unsigned char WriteCom, int Index)
bool SerialFlash_doSelectDie(unsigned char dieNum,int Index)
int SerialFlash_bulkPipeProgram_twoDie(struct CAddressRange* AddrRange, unsigned char* vData, unsigned char modeWrite, unsigned char WriteCom, int Index)
int SerialFlash_bulkPipeRead(struct CAddressRange* AddrRange, unsigned char* vData, unsigned char modeRead, unsigned char ReadCom, int Index)
int SerialFlash_bulkPipeRead_Micron_4die(struct CAddressRange* AddrRange, unsigned char* vData, unsigned char modeRead, unsigned char ReadCom, int Index)
int SerialFlash_bulkPipeRead_twoDie(struct CAddressRange* AddrRange, unsigned char* vData, unsigned char modeRead, unsigned char ReadCom, int Index)
void SerialFlash_SetCancelOperationFlag()
void SerialFlash_ClearCancelOperationFlag()
int SerialFlash_readSR(unsigned char* cSR, int Index)
int SerialFlash_writeSR(unsigned char cSR, int Index)
int SerialFlash_is_protectbits_set(int Index)
bool SerialFlash_StartofOperation(int Index)
bool SerialFlash_EndofOperation(int Index)