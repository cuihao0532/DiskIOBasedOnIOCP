
#include "stdafx.h"
#include "IOs.h"

namespace MyIO
{ 
    CDiskIO::CDiskIO()
    {  
        m_hIocp = NULL;
        m_pEventHandler = NULL; 
    }

    CDiskIO::CDiskIO( IIOFinishEventHandler* pHandler )
    { 
        m_hIocp = NULL;
        m_pEventHandler = pHandler; 
    }

    CDiskIO::~CDiskIO()
    {
        Uninitialize();
    }


    bool CDiskIO::ReadFile(LPCTSTR lpFileName, DWORD dwLowoffset, DWORD dwHighOffset, LPVOID pMem, DWORD dwBytes)
    {
        HANDLE hFile = ::CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        DWORD dwErr = GetLastError();

        bool bRet = false;
        if ( ERROR_SUCCESS == dwErr )
        {
            bRet = Associate2Iocp(hFile);
        }

        if ( bRet )
        {
            POverlappedParam pOverlapped = new OverlappedParam();
            pOverlapped->eMode = OverlappedParam::Operate_Read;
            pOverlapped->hFile = hFile;
            pOverlapped->pMem  = pMem;
            pOverlapped->Offset = dwLowoffset;
            pOverlapped->OffsetHigh = dwHighOffset;

            ::ReadFile(hFile, pMem, dwBytes, NULL, pOverlapped); 
        }
        
        return bRet;
    }



    bool CDiskIO::WriteFile( LPCTSTR lpFileName, LPVOID pMem, DWORD dwBytes )
    {
        // 创建文件（如果存在则失败）
        HANDLE hFile = ::CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_FLAG_OVERLAPPED, NULL);
        DWORD dwErr = GetLastError(); 

        bool bRet = false;
        if ( ERROR_SUCCESS == dwErr )
        {
            // 把文件关联到IOCP
            bRet = Associate2Iocp(hFile);
        } 

        if ( bRet )
        {
            POverlappedParam pOverlapped = new OverlappedParam();
            pOverlapped->eMode = OverlappedParam::Operate_Write;
            pOverlapped->hFile = hFile;
            pOverlapped->pMem  = pMem;
            pOverlapped->Offset = 0;

            ::WriteFile(hFile, pMem, dwBytes, NULL, pOverlapped); 
        } 
            
        return bRet;
    }

    bool CDiskIO::WriteFileAsyn( LPCTSTR lpFileName, LPVOID pMemm, DWORD dwBytes )
    {
        return true;
    }

    bool CDiskIO::Initilize()
    {
        // 创建IOCP
        bool bRet = CreateIocp();
        if ( bRet )
            bRet = InitThreadpool();

        return bRet;
    }

    // 创建IOCP
    bool CDiskIO::CreateIocp()
    {
        if ( !m_hIocp )
            m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

        return ( NULL != m_hIocp );
    }

    // 关联IOCP
    bool CDiskIO::Associate2Iocp( HANDLE hFile )
    {
        if ( !m_hIocp || INVALID_HANDLE_VALUE == hFile || !hFile ) return false;

        HANDLE h = CreateIoCompletionPort(hFile, m_hIocp, 1, 0);
        return ( h == m_hIocp );
    }

    bool CDiskIO::InitThreadpool()
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);

        for ( int i = 0; i < info.dwNumberOfProcessors * 2 + 1; ++ i )
        {
            HANDLE hThread = (HANDLE)_beginthreadex(
                NULL,
                0,
                WorkThread,
                this,
                0,
                NULL);
             
            if ( hThread && INVALID_HANDLE_VALUE != hThread )
                m_vecThreads.push_back(hThread);
            else
                return false;
            
        }

        return true;
    }



    void CDiskIO::Uninitialize()
    {
        for ( int i = 0; i < m_vecThreads.size(); ++ i )
        {
            CloseHandle(m_vecThreads[ i ]);
        }
        m_vecThreads.clear();

        if ( m_hIocp )
        {
            CloseHandle(m_hIocp);
            m_hIocp = NULL;
        }
    }

     

    unsigned int __stdcall WorkThread( void* p )
    {
        CDiskIO* pIO = (CDiskIO*)(p);
        if ( !pIO ) return 0;
       
        DWORD dwCompleteKey = 0;
        DWORD dwNumBytes    = 0;
        POverlappedParam lpOverlapped = NULL;
        IIOFinishEventHandler* pHandler = pIO->m_pEventHandler;

        while ( true )
        {
            BOOL bRet = GetQueuedCompletionStatus(pIO->m_hIocp, &dwNumBytes, &dwCompleteKey, (OVERLAPPED**)( &lpOverlapped ), INFINITE);

            if ( bRet )
            {
                CloseHandle(lpOverlapped->hFile);
                lpOverlapped->hFile = NULL;
                
                if ( pHandler )
                {
                    if ( OverlappedParam::Operate_Write == lpOverlapped->eMode)
                        pHandler->NotifyWriteFinish();
                    else if ( OverlappedParam::Operate_Read == lpOverlapped->eMode )
                        pHandler->NotifyReadFinish();
                }  
            }
            else
            {
                // do something
            } 

            delete lpOverlapped;
            lpOverlapped = NULL;
        }

        return 0;
    }


} // namespace


