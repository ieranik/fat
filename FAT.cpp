#include <iostream>
#include <string>
#include <time.h>
#include "buffer.h"

using namespace std;

struct dir_entry
{
    string name;
    int fn;
    bool isf;
    char time[30];
    int sz;
};

struct dir
{
    int par;
    int ind;
    struct dir_entry lst[7];
};

class FAT
{
private:
    int free;
    int fat[4096];
    struct dir dl[4096];
    unsigned char disk[4096][256];
public:
    FAT()
    {
        int i;
        for(i=0;i<66;i++)
        {
            fat[i]=-1;
        }
        for(i=66;i<4096;i++)
        {
            fat[i]=0;
        }
        struct dir root;
        root.par=-1;
        root.ind=0;
        dl[65]=root;
        free=65;
    }
    int getNextFreeBlock()
    {
        free=(free+1)%4096;
        while(fat[free]!=0)free=(free+1)%4096;
        return free;
    }
    void setA(int blocknum)
    {
        int i;
        for(i=0;i<256;i++)disk[blocknum][i]='a';
        return;
    }

    //done
    int getNumFromDir(int d, string name)
    {
        int i;
        struct dir cur=dl[d];
        for(i=0;i<cur.ind;i++)if(cur.lst[i].name==name)return cur.lst[i].fn;
        return -1;
    }

    //done
    void createFile(string path_name, int file_size)
    {

        int nnum;
        string parp,lname,tmp;
        int pos=path_name.find_last_of("/");
        parp=path_name.substr(0,pos);
        int pardirnum=getFileNumber(parp);
        lname=path_name.substr(pos+1,path_name.length()-pos-1);

        int breq=1+(file_size-1)/256;
        int i,fb,prev;
        for(i=0;i<breq;i++)
        {
            fb=getNextFreeBlock();
            //cout<<"inside cf, fb : "<<fb<<endl;
            setA(fb);
            if(i==0)nnum=fb;
            if(i!=0)fat[prev]=fb;
            if(i==breq-1)fat[fb]=1;
            prev=fb;
        }

        struct dir_entry di;
        di.fn=nnum;
        di.isf=true;
        di.name=lname;
        di.sz=file_size;
        time_t now = time(0);
        struct tm tstruct;
        tstruct = *localtime(&now);
        strftime(di.time, sizeof(di.time), "%Y-%m-%d %X", &tstruct);
        dl[pardirnum].lst[dl[pardirnum].ind]=di;
        dl[pardirnum].ind++;
        return;
    }

    //done
    void createDirectory(string path_name)
    {
        int nnum;
        string parp,lname,tmp;
        int pos=path_name.find_last_of("/");
        parp=path_name.substr(0,pos);
        int pardirnum=getFileNumber(parp);
        lname=path_name.substr(pos+1,path_name.length()-pos-1);

        int fb;
        fb=getNextFreeBlock();
        fat[fb]=1;

        struct dir dd;
        dd.par=pardirnum;
        dd.ind=0;
        dl[fb]=dd;


        struct dir_entry di;
        di.fn=fb;
        di.isf=false;
        di.name=lname;

        time_t now = time(0);
        struct tm tstruct;
        tstruct = *localtime(&now);
        strftime(di.time, sizeof(di.time), "%Y-%m-%d %X", &tstruct);
        dl[pardirnum].lst[dl[pardirnum].ind]=di;
        dl[pardirnum].ind++;
        return;
    }

    //done
    void deleteFileFromParent(int file_number, int parent_nubmer)
    {
        int i;
        for(i=0;i<dl[parent_nubmer].ind;i++)
        {
            if(dl[parent_nubmer].lst[i].fn==file_number)
            {
                break;
            }
        }
        dl[parent_nubmer].lst[i]=dl[parent_nubmer].lst[dl[parent_nubmer].ind-1];
        dl[parent_nubmer].ind--;
        return;
    }

    //done
    void deleteFileNumber(int file_number)
    {
        int tmp;
        while(fat[file_number]!=1)
        {
            tmp=file_number;
            file_number=fat[file_number];
            fat[tmp]=0;
        }
        fat[file_number]=0;
    }

    //done
    void deleteFile(string path_name)
    {
        int f=getFileNumber(path_name);
        int p=getParentNumber(path_name);
        deleteFileNumber(f);
        deleteFileFromParent(f,p);
        return;
    }

    //done
    void deleteDirectoryNumber(int file_number, int parent_number)
    {
        int i;
        for(i=0;i<dl[file_number].ind;i++)
        {
            if(dl[file_number].lst[i].isf==true)
            {
                deleteFileNumber(dl[file_number].lst[i].fn);
            }
            else
            {
                deleteDirectoryNumber(dl[file_number].lst[i].fn,file_number);
            }
        }
        fat[file_number]=0;
        for(i=0;i<dl[parent_number].ind;i++)
        {
            if(dl[parent_number].lst[i].fn==file_number)
            {
                break;
            }
        }
        dl[parent_number].lst[i]=dl[parent_number].lst[dl[parent_number].ind-1];
        dl[parent_number].ind--;
        return;
    }

    //done
    void deleteDirectory(string path_name)
    {
        int f=getFileNumber(path_name);
        int p=getParentNumber(path_name);
        deleteDirectoryNumber(f,p);
        return;
    }

    //done
    int getParentNumber(string path_name)
    {
        string parp,lname,tmp;
        int pos=path_name.find_last_of("/");
        parp=path_name.substr(0,pos);

        return getFileNumber(parp);
    }

    //done
    int getFileNumber(string path_name)
    {
        if(path_name.length()==0)return 65;
        string parp,tmp;
        parp=path_name.substr(1,path_name.length()-1);

        int pos,pardirnum;
        int d=65;
        while(true)
        {
            pos=parp.find_first_of("/");
            if(pos==-1)
            {
                pardirnum=getNumFromDir(d,parp);
                return pardirnum;
            }
            else
            {
                tmp=parp.substr(0,pos);
                parp=parp.substr(pos+1,parp.length()-pos-1);
                d=getNumFromDir(d,tmp);
            }
        }
        return -1;
    }

    //done
    void readFile(int file_number, int length, int offset)
    {
        int i,j,k,bn,fn;
        int sfb,efb,sfo,efo;
        sfb=offset/256;
        efb=(offset+length-1)/256;
        sfo=offset%256;
        efo=(offset+length-1)%256;
        if(sfb==efb)
        {
            bn=sfb;
            fn=file_number;
            while(bn>0)
            {
                bn--;
                file_number=fat[file_number];
            }
            for(i=0;i<=efo-sfo;i++)buffer[i]=disk[file_number][sfo+i];
            file_number=fn;
        }
        else
        {
            j=0;
            bn=sfb;
            fn=file_number;
            while(bn>0)
            {
                bn--;
                file_number=fat[file_number];
            }
            for(i=0;i<=255-sfo;i++,j++)buffer[j]=disk[file_number][sfo+i];
            file_number=fn;

            for(i=sfb+1;i<efb;i++)
            {
                bn=i;
                fn=file_number;
                while(bn>0)
                {
                    bn--;
                    file_number=fat[file_number];
                }
                for(k=0;k<256;k++,j++)buffer[j]=disk[file_number][k];
                file_number=fn;
            }

            bn=efb;
            fn=file_number;
            while(bn>0)
            {
                bn--;
                file_number=fat[file_number];
            }
            for(i=0;i<=efo;i++,j++)buffer[j]=disk[file_number][i];
            file_number=fn;

        }
        return;
    }

    //done
    void writeFile(int file_number, int length, int offset, unsigned char letter)
    {
        int i,j,k,bn,fn;
        int sfb,efb,sfo,efo;
        sfb=offset/256;
        efb=(offset+length-1)/256;
        sfo=offset%256;
        efo=(offset+length-1)%256;
        if(sfb==efb)
        {
            bn=sfb;
            fn=file_number;
            while(bn>0)
            {
                bn--;
                file_number=fat[file_number];
            }
            for(i=0;i<=efo-sfo;i++)disk[file_number][sfo+i]=letter;
            file_number=fn;
        }
        else
        {
            j=0;
            bn=sfb;
            fn=file_number;
            while(bn>0)
            {
                bn--;
                file_number=fat[file_number];
            }
            for(i=0;i<=255-sfo;i++,j++)disk[file_number][sfo+i]=letter;
            file_number=fn;

            for(i=sfb+1;i<efb;i++)
            {
                bn=i;
                fn=file_number;
                while(bn>0)
                {
                    bn--;
                    file_number=fat[file_number];
                }
                for(k=0;k<256;k++,j++)disk[file_number][k]=letter;
                file_number=fn;
            }

            bn=efb;
            fn=file_number;
            while(bn>0)
            {
                bn--;
                file_number=fat[file_number];
            }
            for(i=0;i<=efo;i++,j++)disk[file_number][i]=letter;
            file_number=fn;

        }
        return;
    }

    //cur
    void resizeFile(string path_name, int file_size)
    {
        int sz=1,i,tmp;
        int file_number=getFileNumber(path_name);
        int parent_number=getParentNumber(path_name);
        for(i=0;i<dl[parent_number].ind;i++)
        {
            if(dl[parent_number].lst[i].fn==file_number)
            {
                break;
            }
        }
        dl[parent_number].lst[i].sz=file_size;

        int fn=file_number;
        while(fat[file_number]!=1)
        {
            file_number=fat[file_number];
            sz++;
        }
        int lb=file_number;
        int fb;
        file_number=fn;

        //cout<<lb<<"ok\n";

        file_size=1+(file_size-1)/256;
        if(file_size>sz)
        {
            int cnt=file_size-sz;
            while(cnt>0)
            {
                fb=getNextFreeBlock();
                setA(fb);
                fat[lb]=fb;
                lb=fb;
                cnt--;
            }
            fat[lb]=1;
        }
        else if(file_size<sz)
        {
            int cnt=file_size-1;
            while(cnt>0)
            {
                file_number=fat[file_number];
                cnt--;
            }
            cnt=sz-file_size;
            for(i=0;i<cnt;i++)
            {
                tmp=fat[file_number];
                if(i==0)fat[file_number]=1;
                else fat[file_number]=0;
                file_number=tmp;
            }
        }
        return;
    }

    void printFile(string path_name)
    {
        int f=getFileNumber(path_name);
        while(fat[f]!=1)
        {
            cout<<f<<" -> ";
            f=fat[f];
        }
        cout<<f<<endl<<endl;
        return;
    }

    void printDirectory(string path_name)
    {
        int d=getFileNumber(path_name);
        int i;
        cout<<"File No.   Creation Time           File/Folder   Size    Name\n";
        for(i=0;i<dl[d].ind;i++)
        {
            cout<<dl[d].lst[i].fn<<"\t   ";
            cout<<dl[d].lst[i].time<<"\t";
            if(dl[d].lst[i].isf==true)cout<<"   file          "<<dl[d].lst[i].sz<<"     ";
            else cout<<"   folder                ";
            cout<<dl[d].lst[i].name<<endl;
        }
        cout<<endl;
        return;
    }

    void printFAT(int start_index, int end_index)
    {
        int i;
        for(i=start_index;i<=end_index;i++)
        {
            cout<<"Block No. "<<i<<" -> "<<fat[i]<<endl;
        }
        cout<<endl;
    }
};
