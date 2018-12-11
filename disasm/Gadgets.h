#ifndef _GADGETS_H
#define _GADGETS_H 1


#include<bits/stdc++.h>
#include<stdio.h>
#include<sys/mman.h>
#include<unistd.h>
#include<fcntl.h>
#include<elf.h>
#include<capstone/capstone.h>


int GetGadgets(unsigned char *retptr, unsigned long RetAddress, unsigned long N);
int GetAllGadgets(unsigned char *inst, unsigned long TextSize, unsigned long EntryAddress, unsigned long N);


#endif
