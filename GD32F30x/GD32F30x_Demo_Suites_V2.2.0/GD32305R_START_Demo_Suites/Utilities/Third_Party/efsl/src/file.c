/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : file.c                                                           *
* Description : This file contains functions dealing with files such as:      *
*               fopen, fread and fwrite.                                      *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; version 2                     *
* of the License.                                                             *
                                                                              *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* As a special exception, if other files instantiate templates or             *
* use macros or inline functions from this file, or you compile this          *
* file and link it with other works to produce a work based on this file,     *
* this file does not by itself cause the resulting work to be covered         *
* by the GNU General Public License. However the source code for this         *
* file must still be made available in accordance with section (3) of         *
* the GNU General Public License.                                             *
*                                                                             *
* This exception does not invalidate any other reasons why a work based       *
* on this file might be covered by the GNU General Public License.            *
*                                                                             *
*                                                    (c)2006 Lennart Yseboodt *
*                                                    (c)2006 Michael De Nil   *
\*****************************************************************************/

/*****************************************************************************/

#include "file.h"
/*****************************************************************************/

/* ****************************************************************************  
 * euint32 file_fread(File *file,euint32 offset, euint32 size,euint8 *buf)
 * Description: This function reads 'size' bytes from 'file' starting at
 * 'offset' and puts the result in '*buf'.
 * Return value: amount of bytes actually read (can differ from the given
 * size when the file was smaller
*/
euint32 file_fread(File *file,euint32 offset, euint32 size,euint8 *buf)
{
	euint32 bytes_read=0,size_left=size,coffset=offset;
	euint32 cclus,csec,cbyte;
	euint32 rclus,rsec;
	euint32 btr;
	euint8 *tbuf;
		
	if(!file_getAttr(file,FILE_STATUS_OPEN))return(0);
	
	if(offset>=file->FileSize)
		size_left=0; /* Offset check */
	
	if( (offset+size > file->FileSize) && size_left!=0)
		size_left=file->FileSize-offset;
	
	while(size_left>0){
	
		cclus = coffset/(512*file->fs->volumeId.SectorsPerCluster);
		csec = (coffset/(512))%file->fs->volumeId.SectorsPerCluster;
		cbyte = coffset%512;
		
		if(cbyte!=0 || size_left<512){
			btr = 512-(coffset%512)>=size_left?size_left:512-(coffset%512);
		}else{
			btr = 512;
		}

		if((fat_LogicToDiscCluster(file->fs,&(file->Cache),cclus))!=0){
			return(0);
		}
		rclus=file->Cache.DiscCluster;
		rsec=fs_clusterToSector(file->fs,rclus);
		
		
		if(btr==512){
			/*part_readBuf(file->fs->part,rsec+csec,buf+bytes_read);*/
			part_directSectorRead(file->fs->part,rsec+csec,buf+bytes_read);
		}else{
			/*part_readBuf(file->fs->part,rsec+csec,tbuf);*/
			tbuf = part_getSect(file->fs->part,rsec+csec,IOM_MODE_READONLY);
			/* correction to original EFSL */
			if (tbuf)
			{
			  memCpy(tbuf+(coffset%512),buf+bytes_read,btr);
			  part_relSect(file->fs->part,tbuf);
			}
			else return 0;
		}
		
		coffset+=btr;
		bytes_read+=btr;
		size_left-=btr;
	}
		
	return(bytes_read);
}
/*****************************************************************************/

/* ****************************************************************************  
 * euint32 file_read (File *file,euint32 size,euint8 *buf)
 * Description: This function reads from a file, taking the FilePtr into account
 * and advancing it according to the freadcall.
 * Return value: Value obtained from fread
*/
euint32 file_read(File *file,euint32 size,euint8 *buf)
{
	euint32 r;
	
	r=file_fread(file,file->FilePtr,size,buf);
	file->FilePtr+=r;
	return(r);
}
/*****************************************************************************/

/* ****************************************************************************  
 * euint32 file_write(File *file, euint32 size,euint8 *buf)
 * Description: This function writes to a file, taking FilePtr into account
 * and advancing it according to the fwritecall.
 * Return value: Value obtained from fread
*/
euint32 file_write(File *file, euint32 size,euint8 *buf)
{
	euint32 r;
	
	r=file_fwrite(file,file->FilePtr,size,buf);
	file->FilePtr+=r;
	return(r);
}
/*****************************************************************************/

/* ****************************************************************************  
 * esint16 file_setpos(File *file,euint32 pos)
 * Description: This function does a sanity check on the requested position
 * and changes the fileptr accordingly.
 * Return value: 0 on success and -1 on failure.
*/
esint16 file_setpos(File *file,euint32 pos)
{
	if(pos<=file->FileSize){
		file->FilePtr=pos;
		return(0);
	}
	return(-1);
}
/*****************************************************************************/

/* ****************************************************************************  
 * euint32 file_fwrite(File* file,euint32 offset,euint32 size,euint8* buf)
 * Description: This function writes to a file, at offset 'offset' and size 'size'.
 * It also updates the FileSize in the object, and discstructure.
 * Return value: Bytes actually written.
*/
euint32 file_fwrite(File* file,euint32 offset,euint32 size,euint8* buf)
{
	euint32 need_cluster;
	euint32 cclus,csec,cbyte;
	euint32 size_left=size,bytes_written=0;
	euint32 rclus,rsec;
	euint32 coffset=offset;
	euint16 btr;
	euint8 *tbuf;

	if(!file_getAttr(file,FILE_STATUS_OPEN) || !file_getAttr(file,FILE_STATUS_WRITE))return(0);
	
	if(offset>file->FileSize){
		offset=file->FileSize;
	}
	
	need_cluster = file_requiredCluster(file,offset,size);
	
	if(need_cluster){
		if(fat_allocClusterChain(file->fs,&(file->Cache),need_cluster+CLUSTER_PREALLOC_FILE)!=0){
			return(0);
		}
	}
	
	while(size_left>0){
	
		cclus = coffset/(512*file->fs->volumeId.SectorsPerCluster);
		csec = (coffset/(512))%file->fs->volumeId.SectorsPerCluster;
		cbyte = coffset%512;
		
		if(cbyte!=0 || size_left<512){
			btr = 512-(coffset%512)>=size_left?size_left:512-(coffset%512);
		}else{
			btr = 512;
		}

		if((fat_LogicToDiscCluster(file->fs,&(file->Cache),cclus))!=0){
			file->FileSize+=bytes_written;
			dir_setFileSize(file->fs,&(file->Location),file->FileSize);
			return(bytes_written);
		}
		rclus=file->Cache.DiscCluster;
		rsec=fs_clusterToSector(file->fs,rclus);
		
		if(btr==512){
			/*part_writeBuf(file->fs->part,rsec+csec,buf+bytes_written);*/
			part_directSectorWrite(file->fs->part,rsec+csec,buf+bytes_written);
		}else{
			/*part_readBuf(file->fs->part,rsec+csec,tbuf);*/
			tbuf = part_getSect(file->fs->part,rsec+csec,IOM_MODE_READWRITE);
			memCpy(buf+bytes_written,tbuf+(coffset%512),btr);
			/*part_writeBuf(file->fs->part,rsec+csec,tbuf);*/
			part_relSect(file->fs->part,tbuf);
		}
		
		coffset+=btr;
		bytes_written+=btr;
		size_left-=btr;
	}
	
	if(bytes_written>file->FileSize-offset){
		file->FileSize+=bytes_written-(file->FileSize-offset);
    }
	
	return(bytes_written);	
}

/* ***************************************************************************\
 * signed eint8 file_fopen(FileSystem *fs,File* file,eint8* filename)      
 * Description: This functions opens a file.                               
 * This function is about to be redesigned. No Docs.                       
 * Return value:
*/
esint8 file_fopen(File* file,FileSystem *fs,eint8* filename,eint8 mode)
{
    FileLocation loc;
    FileRecord wtmp;
    eint8 fatfilename[11];
    euint32 sec;

    dir_getFatFileName(filename,fatfilename);
	
    switch(mode)
	{
        case MODE_READ:
            if(fs_findFile(fs,filename,&loc,0)==1)
			{
                dir_getFileStructure(fs,&(file->DirEntry), &loc);
                file_initFile(file,fs,&loc);
				file_setAttr(file,FILE_STATUS_OPEN,1);
				file_setAttr(file,FILE_STATUS_WRITE,0);
                return(0);
            }
            return(-1);
            //break;
        case MODE_WRITE:
            if(fs_findFile(fs,filename,&loc,&sec)) /* File may NOT exist, but parent HAS to exist */
			{
                return(-2);
			}
			if(sec==0){ /* Parent dir does not exist */
 				return(-4);
			}
            if(fs_findFreeFile(fs,filename,&loc,0))
			{
                dir_createDefaultEntry(fs,&wtmp,fatfilename);
                dir_createDirectoryEntry(fs,&wtmp,&loc);
                memCpy(&wtmp,&(file->DirEntry),sizeof(wtmp));
				file_initFile(file,fs,&loc);
                sec=fs_getNextFreeCluster(file->fs,fs_giveFreeClusterHint(file->fs));
                dir_setFirstCluster(file->fs,&(file->Location),sec);
                fs_setFirstClusterInDirEntry(&(file->DirEntry),sec);
                fs_initClusterChain(fs,&(file->Cache),sec);
                fat_setNextClusterAddress(fs,sec,fat_giveEocMarker(fs));
				file_setAttr(file,FILE_STATUS_OPEN,1);
				file_setAttr(file,FILE_STATUS_WRITE,1);
            	return(0);
			}
            else
			{
                return(-3);
			}
            //break;
        case MODE_APPEND:
			if(fs_findFile(fs,filename,&loc,0)==1) /* File exists */
			{
				dir_getFileStructure(fs,&(file->DirEntry), &loc);
				file_initFile(file,fs,&loc);
				if(file->Cache.FirstCluster==0){
					sec=fs_getNextFreeCluster(file->fs,fs_giveFreeClusterHint(file->fs));
					dir_setFirstCluster(file->fs,&(file->Location),sec);
					fs_setFirstClusterInDirEntry(&(file->DirEntry),sec);
					fat_setNextClusterAddress(fs,sec,fat_giveEocMarker(fs));
					file_initFile(file,fs,&loc);
				}
				file_setpos(file,file->FileSize);
				file_setAttr(file,FILE_STATUS_OPEN,1);
				file_setAttr(file,FILE_STATUS_WRITE,1);
			}
			else /* File does not excist */
			{
				if(fs_findFreeFile(fs,filename,&loc,0))
				{
					dir_createDefaultEntry(fs,&wtmp,fatfilename);
					dir_createDirectoryEntry(fs,&wtmp,&loc);
					memCpy(&wtmp,&(file->DirEntry),sizeof(wtmp));
					file_initFile(file,fs,&loc);
					sec=fs_getNextFreeCluster(file->fs,fs_giveFreeClusterHint(file->fs));
					dir_setFirstCluster(file->fs,&(file->Location),sec);
	                fs_setFirstClusterInDirEntry(&(file->DirEntry),sec);
    	            fs_initClusterChain(fs,&(file->Cache),sec);
					fat_setNextClusterAddress(fs,sec,fat_giveEocMarker(fs));
					file_setAttr(file,FILE_STATUS_OPEN,1);
					file_setAttr(file,FILE_STATUS_WRITE,1);
				}
				else
				{
					return(-3);
				}
			}
			return(0);
            //break;
        default:
            return(-4);
          //  break;
    }
    //return(-5);
}
/*****************************************************************************/

/* ****************************************************************************  
 * esint8 file_fclose(File *file)
 * Description: This function closes a file, by clearing the object.
 * Return value: 0 on success.
*/
esint8 file_fclose(File *file)
{
	if(fs_hasTimeSupport()){
		file->DirEntry.AccessDate = time_getDate();
		if(file_getAttr(file,FILE_STATUS_WRITE)){
			file->DirEntry.FileSize = file->FileSize;
			file->DirEntry.WriteDate = file->DirEntry.AccessDate;
			file->DirEntry.WriteTime = time_getTime();
		}
		dir_updateDirectoryEntry(file->fs,&(file->DirEntry),&(file->Location));
	}else{
		if(file_getAttr(file,FILE_STATUS_WRITE)){
			dir_setFileSize(file->fs,&(file->Location),file->FileSize);
		}
	}
	
	memClr(file,sizeof(*file));
	file_setAttr(file,FILE_STATUS_OPEN,0);
	file_setAttr(file,FILE_STATUS_WRITE,0);
	return(0);
}


/* ****************************************************************************  
 * void file_initFile(File *file, FileSystem *fs, FileLocation *loc)
 * Description: This function initialises a new file object, by filling in
 * the fs pointer, filesize (note, that DirEntry must already be filled in)
 * and known cache parameters.
 * Return value: void
*/
void file_initFile(File *file, FileSystem *fs, FileLocation *loc)
{
	file->fs=fs;
	file->FileSize=file->DirEntry.FileSize;
	file->FilePtr=0;
	file->Location.Sector=loc->Sector;
	file->Location.Offset=loc->Offset;
	file->Cache.Linear=0;
	file->Cache.FirstCluster=(((euint32)file->DirEntry.FirstClusterHigh)<<16)+ 
	                                    file->DirEntry.FirstClusterLow;
	file->Cache.LastCluster=0; 
	file->Cache.LogicCluster=0;
	file->Cache.DiscCluster=file->Cache.FirstCluster;
}
/*****************************************************************************/

/* ****************************************************************************  
 * euint8* file_normalToFatName(eint8* filename,eint8* fatfilename)
 * Description: This function converts a human readable filename (limited to 
 * 8.3 eint8 character) to a valid FAT (not VFAT) filename. Invalid characters are 
 * changed to capital X and only the first 11 characters are used.
 * Furthermore all letters are capitalised.
 * Return value: pointer after the filename
*/
eint8* file_normalToFatName(eint8* filename,eint8* fatfilename)
{
	euint8 c,dot=0,vc=0;
	
	for(c=0;c<11;c++)fatfilename[c]=' ';
	
	c=0;
	
	if(*filename == '.'){
		fatfilename[0]='.';
		vc++; 
		if(*(filename+1) == '.'){
			fatfilename[1]='.';
			filename+=2;
		}else{
			filename++;
		}
	}else{
		while(*filename != '\0' && /**filename != ' ' &&*/ *filename != '/'){   //issue found with short filenames <8 chars
			if(*filename=='.' && !dot){
				dot=1;
				c=8;
                              
			}else{
				if(dot){
					if(c<=10){
						fatfilename[c]=file_validateChar(*filename);
						c++; 
					}
				}else{
					if(c<=7){
						fatfilename[c]=file_validateChar(*filename);
						c++; vc++;
					}
				}
			}
			filename++;
		}
	}
	
	if(vc>0){
		if(*filename=='\0'){
			return(filename);
		}else{
			return(filename+1);
		}
	}else{
		return(0);
	}
}
/*****************************************************************************/

/* ****************************************************************************  
 *
 * Description: This function takes the character c, and if it is not a       *
 * valid FAT Filename character returns X. If it is a lowercase letter the    *
 * uppercase equivalent is returned. The remaining characters are returned    *
 * as they are.      
 * Return value: The validated char
*/
euint8 file_validateChar(euint8 c)
{
    if( (c<0x20) || (c>0x20&&c<0x30&&c!='-') || (c>0x39&&c<0x41) || (c>0x5A&&c<0x61&&c!='_') ||	(c>0x7A&&c!='~') )
		return(0x58);
    if( c>=0x61 && c<=0x7A )
		return(c-32);

	return(c);
}
/*****************************************************************************/

/* ****************************************************************************  
 * void ioman_setAttr(IOManager *ioman,euint16 bufplace,euint8 attribute,euint8 val)
 * Description: This sets the attribute of 'bufplace' to the given value (binary).
 *
 * Return value: void
*/
void file_setAttr(File* file,euint8 attribute,euint8 val)
{
	if(val){
		file->FileStatus|=1<<attribute;
	}else{
		file->FileStatus&=~(1<<attribute);
	}
}
/*****************************************************************************/

/* ****************************************************************************  
 * euint8 ioman_getAttr(IOManager *ioman,euint16 bufplace,euint8 attribute)
 * Description: This function retrieves an attribute from the bufstat array.
 * It returns nonzero when it attribute is true and 0 when it is false.
 * Please note, I said "nonzero", not 1.
 * Return value: Attribute.
*/
euint8 file_getAttr(File* file,euint8 attribute)
{
	return(file->FileStatus&(1<<attribute));
}
/*****************************************************************************/

euint32 file_requiredCluster(File *file,euint32 offset, euint32 size)
{
	euint32 clusters_required,clustersize;
	euint32 hc;

	if((offset+size)>file->FileSize){
		if(file->Cache.ClusterCount==0){ /* Number of cluster unknown */
			hc = fat_countClustersInChain(file->fs,file->Cache.FirstCluster);
			file->Cache.ClusterCount = hc;
		}else{
			hc = file->Cache.ClusterCount; /* This better be right */
		}
		clustersize = file->fs->volumeId.BytesPerSector * file->fs->volumeId.SectorsPerCluster;
		if((size-file->FileSize+offset)>
		   ((hc-((file->FileSize+clustersize-1)/clustersize))*clustersize)){
			clusters_required = (((offset+size)-(hc*clustersize))+clustersize-1)/clustersize;
		}else{
			clusters_required = 0;
		}
	}else{
		clusters_required = 0;
	}
	return(clusters_required);
}
