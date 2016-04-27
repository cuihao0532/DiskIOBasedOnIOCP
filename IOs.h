
#include <vector>
using std::vector;

#pragma once

namespace MyIO
{
    __interface IDiskIO
    {
        bool WriteFile(LPCTSTR lpFileName, LPVOID pMem, DWORD dwBytes);
        bool WriteFileAsyn(LPCTSTR lpFileName, LPVOID pMemm, DWORD dwBytes);
    };

    __interface IIOFinishEventHandler
    {
        void NotifyWriteFinish();
        void NotifyReadFinish();
    };

    typedef struct _tagOverlappedParam : public OVERLAPPED
    {
    public:
        _tagOverlappedParam() { memset(this, 0, sizeof(*this)); }

        enum eOperateMode
        {
            Operate_Read = 0,
            Operate_Write = 1,
            Operate_Useless
        };

        eOperateMode eMode;
        HANDLE hFile;
        void*  pMem;

    }OverlappedParam, *POverlappedParam;




    class CDiskIO : public IDiskIO
    {
    public:
        CDiskIO();  
        CDiskIO(IIOFinishEventHandler* pHandler);
        virtual ~CDiskIO();


    public:
        enum eOpenFileMode
        {
            Open_File_Read = GENERIC_READ,
            Open_File_Write = GENERIC_WRITE,
            Open_File_Read_Write = GENERIC_READ | GENERIC_WRITE
        };


        /************************************************************************/
        /* ��ʼ��
         * ����IOCP �����̳߳����߳�
        /************************************************************************/
        bool  Initilize();


        /************************************************************************/
        /* д�ļ��������ļ��������򴴽��ļ���������ʧ��
        /************************************************************************/
        bool  WriteFile(LPCTSTR lpFileName, LPVOID pMem, DWORD dwBytes);



        /************************************************************************/
        /* δʵ��
        /************************************************************************/
        bool  WriteFileAsyn(LPCTSTR lpFileName, LPVOID pMemm, DWORD dwBytes);



        /************************************************************************/
        /* ���ļ����������ļ��������
        /************************************************************************/
        bool  ReadFile(LPCTSTR lpFileName, DWORD dwLowoffset, DWORD dwHighOffset, LPVOID pMem, DWORD dwBytes);





    protected:
        HANDLE m_hIocp; 
        IIOFinishEventHandler* m_pEventHandler; 
        vector<HANDLE> m_vecThreads;


    protected:
        bool CreateIocp();
        bool Associate2Iocp(HANDLE hFile);
        bool InitThreadpool(); 
        void Uninitialize();

        
        friend unsigned int __stdcall WorkThread(void* p);

    };







} // namespace
