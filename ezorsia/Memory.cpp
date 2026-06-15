#include "stdafx.h"
#include "Memory.h"
#include "detours.h"
// #pragma optimize("", off) //non-optimized function for testing purposes
bool Memory::UseVirtuProtect = true;

bool Memory::SetHook(bool attach, void** ptrTarget, void* ptrDetour)
{
    if (DetourTransactionBegin() != NO_ERROR)
    {
        return false;
    }

    HANDLE pCurThread = GetCurrentThread();

    if (DetourUpdateThread(pCurThread) == NO_ERROR)
    {
        auto pDetourFunc = attach ? DetourAttach : DetourDetach;

        if (pDetourFunc(ptrTarget, ptrDetour) == NO_ERROR)
        {
            if (DetourTransactionCommit() == NO_ERROR)
            {
                return true;
            }
        }
    }

    DetourTransactionAbort();
    return false;
}

void Memory::FillBytes(const DWORD dwOriginAddress, const unsigned char ucValue, const int nCount) {
    if (UseVirtuProtect) {
        DWORD dwOldProtect;
        VirtualProtect((LPVOID)dwOriginAddress, nCount, PAGE_EXECUTE_READWRITE, &dwOldProtect); // thanks colaMint, joo, and stelmo for informing me of using virtualprotect
        memset((void*)dwOriginAddress, ucValue, nCount);
        VirtualProtect((LPVOID)dwOriginAddress, nCount, dwOldProtect, &dwOldProtect);
    }
    else { memset((void*)dwOriginAddress, ucValue, nCount); }
}

void Memory::WriteString(const DWORD dwOriginAddress, const char* sContent) {
    const size_t nSize = strlen(sContent);
    if (UseVirtuProtect) {
        DWORD dwOldProtect;
        VirtualProtect((LPVOID)dwOriginAddress, nSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
        memcpy((void*)dwOriginAddress, sContent, nSize);
        VirtualProtect((LPVOID)dwOriginAddress, nSize, dwOldProtect, &dwOldProtect);
    }
    else { memcpy((void*)dwOriginAddress, sContent, nSize); }
}

void Memory::WriteByte(const DWORD dwOriginAddress, const unsigned char ucValue) {
    if (UseVirtuProtect) {
        DWORD dwOldProtect;
        VirtualProtect((LPVOID)dwOriginAddress, sizeof(unsigned char), PAGE_EXECUTE_READWRITE, &dwOldProtect);
        *(unsigned char*)dwOriginAddress = ucValue;
        VirtualProtect((LPVOID)dwOriginAddress, sizeof(unsigned char), dwOldProtect, &dwOldProtect);
    }
    else { *(unsigned char*)dwOriginAddress = ucValue; }
}

void Memory::WriteShort(const DWORD dwOriginAddress, const unsigned short usValue) {
    if (UseVirtuProtect) {
        DWORD dwOldProtect;
        VirtualProtect((LPVOID)dwOriginAddress, sizeof(unsigned short), PAGE_EXECUTE_READWRITE, &dwOldProtect);
        *(unsigned short*)dwOriginAddress = usValue;
        VirtualProtect((LPVOID)dwOriginAddress, sizeof(unsigned short), dwOldProtect, &dwOldProtect);
    }
    else { *(unsigned short*)dwOriginAddress = usValue; }
}

void Memory::WriteInt(const DWORD dwOriginAddress, const unsigned int dwValue) {
    if (UseVirtuProtect) {
        DWORD dwOldProtect;
        VirtualProtect((LPVOID)dwOriginAddress, sizeof(unsigned int), PAGE_EXECUTE_READWRITE, &dwOldProtect);
        *(unsigned int*)dwOriginAddress = dwValue;
        VirtualProtect((LPVOID)dwOriginAddress, sizeof(unsigned int), dwOldProtect, &dwOldProtect);
    }
    else { *(unsigned int*)dwOriginAddress = dwValue; }
}

void Memory::WriteDouble(const DWORD dwOriginAddress, const double dwValue) {
    if (UseVirtuProtect) {
        DWORD dwOldProtect;
        VirtualProtect((LPVOID)dwOriginAddress, sizeof(double), PAGE_EXECUTE_READWRITE, &dwOldProtect);
        *(double*)dwOriginAddress = dwValue;
        VirtualProtect((LPVOID)dwOriginAddress, sizeof(double), dwOldProtect, &dwOldProtect);
    }
    else { *(double*)dwOriginAddress = dwValue; }
}

void Memory::WriteByteArray(const DWORD dwOriginAddress, const unsigned char* ucValue, const int ucValueSize) {
    if (UseVirtuProtect) {
        for (int i = 0; i < ucValueSize; i++) {
            const DWORD newAddr = dwOriginAddress + i;
            DWORD dwOldProtect;
            VirtualProtect((LPVOID)newAddr, sizeof(unsigned char), PAGE_EXECUTE_READWRITE, &dwOldProtect);
            *(unsigned char*)newAddr = ucValue[i];
            VirtualProtect((LPVOID)newAddr, sizeof(unsigned char), dwOldProtect, &dwOldProtect);
        }
    }
    else {
        for (int i = 0; i < ucValueSize; i++) { const DWORD newAddr = dwOriginAddress + i; *(unsigned char*)newAddr = ucValue[i]; }
    }
}

void Memory::CodeCave(void* ptrCodeCave, const DWORD dwOriginAddress, const int nNOPCount) { // tested and working
	__try {
		if (nNOPCount) FillBytes(dwOriginAddress, 0x90, nNOPCount); // create space for the jmp
		WriteByte(dwOriginAddress, 0xe9); // jmp instruction
		WriteInt(dwOriginAddress + 1, (int)(((int)ptrCodeCave - (int)dwOriginAddress) - 5)); // [jmp(1 byte)][address(4 bytes)] //this means you need to clear a space of at least 5 bytes (nNOPCount bytes)
	} __except (EXCEPTION_EXECUTE_HANDLER) {}
}
// #pragma optimize("", on)

void Memory::WriteValue(const DWORD dwOriginAddress, const unsigned int dwValue) {
    if (UseVirtuProtect) {
        DWORD dwOldProtect;
        VirtualProtect((LPVOID)dwOriginAddress, sizeof(unsigned int), PAGE_EXECUTE_READWRITE, &dwOldProtect);
        *(unsigned int*)dwOriginAddress = dwValue;
        VirtualProtect((LPVOID)dwOriginAddress, sizeof(unsigned int), dwOldProtect, &dwOldProtect);
    }
    else { *(unsigned int*)dwOriginAddress = dwValue; }
}

void Memory::PatchNop(DWORD dwAddress, UINT nCount)
{
	if (nCount != 0) {
		FillBytes(dwAddress, 0x90, nCount);
	}
}

unsigned int Memory::FindAoB(const char* sAoB, DWORD dwStartAddress, DWORD dwEndAddress, int nSkip) {
    constexpr DWORD PE_START = 0x401000;
    constexpr int MAX_BUFFER = 1024;
    unsigned char pBuff[MAX_BUFFER] = { 0x00 };
    bool aMask[MAX_BUFFER] = { 0x00 };
    unsigned int uSize = ReadAoB(sAoB, pBuff, aMask);
    unsigned int i, j;
    int nSkipped = 0;

    if (uSize > 0) {
        dwStartAddress = dwStartAddress ? dwStartAddress : PE_START;
        dwEndAddress = dwEndAddress ? dwEndAddress : 0x07FFFFFF;

        __try {
            for (i = dwStartAddress; i < (dwEndAddress - uSize); i++) {
                for (j = 0; j < uSize; j++) {
                    if (aMask[j]) {
                        continue;
                    }
                    if (pBuff[j] != *(unsigned char*)(i + j)) {
                        break;
                    }
                }
                if (j == uSize) {
                    if (nSkipped++ >= nSkip) {
                        return i;
                    }
                }
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
        }
    }

    return 0;
}

unsigned int Memory::ReadAoB(const char* sAoB, unsigned char* pBuff, bool* pMask) {
    unsigned char bOp;
    int i = 0, j = 0;

    while ((bOp = (BYTE)sAoB[j]), bOp) {
        unsigned char bFlag = (i % 2 == 0) ? 4 : 0;

        if (bOp >= '0' && bOp <= '9')
            pBuff[i++ / 2] += (bOp - '0') << bFlag;
        else if (bOp >= 'A' && bOp <= 'F')
            pBuff[i++ / 2] += (bOp - 'A' + 0xA) << bFlag;
        else if (bOp >= 'a' && bOp <= 'f')
            pBuff[i++ / 2] += (bOp - 'a' + 0xA) << bFlag;
        else if (bOp == '?') {
            pBuff[i / 2] = 0xFF;
            pMask[i / 2] = true;
            i++;
        }
        j++;
    }

    return (i % 2 == 0) ? (i / 2) : -1;
}
