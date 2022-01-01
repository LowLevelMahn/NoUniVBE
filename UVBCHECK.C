// build with Borland C++ 3.1: BCC.EXE -mc uvbcheck.c
// BEWARE: needs to be build with a FAR-Data-Pointer memory model (compact or large) to work correct
// or else the far pointers get crippled - mostly wrong OEM-Strings etc. will be the result (Thanks to FalcoSoft/Vogons-Forum)

#include <stdio.h>
#include <string.h>
#include <dos.h>

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)

#define STATIC_ASSERT(COND) struct TOKENPASTE2(ASSERT_STRUCT_, __LINE__){char static_assertion[(COND)?1:-1];};
#define OFFSETOF(type, field)((unsigned long) (&(((type*)0)->field)))

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

STATIC_ASSERT(sizeof(uint8_t)==1)
STATIC_ASSERT(sizeof(uint16_t)==2)
STATIC_ASSERT(sizeof(uint32_t)==4)

#pragma pack(1)

struct FarPointer
{
  uint16_t offset;
  uint16_t segment;
};

char far* to_str(struct FarPointer* ptr)
{
  char far* str_ptr = 0;
  str_ptr = MK_FP(ptr->segment, ptr->offset);
  return str_ptr;
}

struct vbeinfoblock  {
  char VbeSignature[4];
  uint16_t VbeVersion;
  struct FarPointer OemStringPtr;
  uint32_t Capabilities;
  struct FarPointer VideoModePtr;
  uint16_t TotalMemory;
  uint16_t OemSoftwareRev;
  struct FarPointer OemVendorNamePtr;
  struct FarPointer OemProductNamePtr;
  struct FarPointer OemProductRevPtr;
  uint8_t Reserved1[222];
  uint8_t OemData[256];
};

STATIC_ASSERT(sizeof(struct vbeinfoblock)==512)
STATIC_ASSERT(OFFSETOF(struct vbeinfoblock, VbeSignature)==0x00)
STATIC_ASSERT(OFFSETOF(struct vbeinfoblock, VbeVersion)==0x04)
STATIC_ASSERT(OFFSETOF(struct vbeinfoblock, OemStringPtr)==0x06)
STATIC_ASSERT(OFFSETOF(struct vbeinfoblock, Capabilities)==0x0A)
STATIC_ASSERT(OFFSETOF(struct vbeinfoblock, VideoModePtr)==0x0E)
STATIC_ASSERT(OFFSETOF(struct vbeinfoblock, TotalMemory)==0x12)  
STATIC_ASSERT(OFFSETOF(struct vbeinfoblock, OemSoftwareRev)==0x14)
STATIC_ASSERT(OFFSETOF(struct vbeinfoblock, OemVendorNamePtr)==0x16)
STATIC_ASSERT(OFFSETOF(struct vbeinfoblock, OemProductNamePtr)==0x1A)
STATIC_ASSERT(OFFSETOF(struct vbeinfoblock, OemProductRevPtr)==0x1E)
STATIC_ASSERT(OFFSETOF(struct vbeinfoblock, Reserved1)==0x22)
STATIC_ASSERT(OFFSETOF(struct vbeinfoblock, OemData)==0x100)

struct UniVBE_53a_data_t
{
  char card_info[80];
  char dac_info[50];
  char clock_info[50];
  
  uint8_t ciphered_registered_user[62];
  uint8_t ciphered_serial_no[21];
  
  char version_major_minor[2]; // "53"
  char release_date[15]; // "Jan 24 1997"

  uint16_t version; // license
  uint16_t unknown2;
  
  uint16_t enabled;
  
  uint16_t vbe2_support;
  uint16_t native_mode_centering;
  uint16_t lfb_support;
  uint16_t vbe2_palette_functions;
  uint16_t vbe2_32bit_interface;
  uint16_t directx2_compatiblity_mode;
};

STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, card_info)==0x00)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, dac_info)==0x50)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, clock_info)==0x82)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, ciphered_registered_user)==0xB4)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, ciphered_serial_no)==0xF2)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, version_major_minor)==0x107)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, release_date)==0x109)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, version)==0x118)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, unknown2)==0x11A)  
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, enabled)==0x11C)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, vbe2_support)==0x11E)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, native_mode_centering)==0x120)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, lfb_support)==0x122)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, vbe2_palette_functions)==0x124)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, vbe2_32bit_interface)==0x126)
STATIC_ASSERT(OFFSETOF(struct UniVBE_53a_data_t, directx2_compatiblity_mode)==0x128)

#pragma pack()

static void DumpHex(const void far* data, size_t size) {
  char ascii[17];
  size_t i, j;
  ascii[16] = '\0';
  for (i = 0; i < size; ++i) {
    printf("%02X ", ((unsigned char*)data)[i]);
    if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
      ascii[i % 16] = ((unsigned char*)data)[i];
    } else {
      ascii[i % 16] = '.';
    }
    if ((i+1) % 8 == 0 || i+1 == size) {
      printf(" ");
      if ((i+1) % 16 == 0) {
        printf("|  %s \n", ascii);
      } else if (i+1 == size) {
        ascii[(i+1) % 16] = '\0';
        if ((i+1) % 16 <= 8) {
          printf(" ");
        }
        for (j = (i+1) % 16; j < 16; ++j) {
          printf("   ");
        }
        printf("|  %s \n", ascii);
      }
    }
  }
}

#define UV_FEATURE_ON 0
#define UV_FEATURE_OFF 1

const char* on_off_str(int on_off_)
{
  return (on_off_ == UV_FEATURE_ON) ? "ON":"OFF";
}

const char* SIGNATURE = "Universal VESA VBE";

static void deciper_text(char* text_)
{
  int i = 0;
  int len = strlen(text_);
  //assert(len < 256);
  for (i = len - 1; i >= 0; --i)
  {
      text_[i] ^= 0xff - (i << 2);
  }
}

int main()
{
  struct vbeinfoblock VIB;
  union REGS inregs;
  union REGS outregs;
  struct SREGS sregs;
  uint16_t ax_reg = 0;
  uint16_t drv_ofs = 0;
  uint16_t drv_seg = 0;
  char far* oem_str = 0;
  const char* starts_with = "Universal VESA VBE";
  struct UniVBE_53a_data_t far* univbe_data = 0;
  struct FarPointer vec10h;
  uint8_t far* signature = 0;
  struct FarPointer vec2fh;
  char buffer[100];

  memset(&VIB, 0, sizeof(VIB));
  memset(&inregs, 0, sizeof(inregs));
  memset(&outregs, 0, sizeof(outregs));
  memset(&sregs, 0, sizeof(sregs));
  memset(&vec10h, 0, sizeof(vec10h));
  memset(&vec2fh, 0, sizeof(vec2fh));
  memset(&buffer, 0, sizeof(buffer));

  memcpy(VIB.VbeSignature, "VBE2", 4);
  //memcpy(VIB.VbeSignature, "VESA", 4);
  
  printf("Init signature: %.4s\n", VIB.VbeSignature);

  inregs.x.ax = 0x4F00;

  // gets handled in UniVBE int10h handler someway
  inregs.x.bx = 0x1234;
  inregs.x.cx = 0x4321;

  inregs.x.di = FP_OFF(&VIB);
  sregs.es = FP_SEG(&VIB);

  printf("int 10h/ax=4F00h\n");
  ax_reg = int86x(0x10, &inregs, &outregs, &sregs);
  printf("ax_reg: 0x%04X\n", ax_reg);
  printf("bx_reg: 0x%04X\n", outregs.x.bx);
  printf("cx_reg: 0x%04X\n", outregs.x.cx);
  printf("dx_reg: 0x%04X\n", outregs.x.dx);
  if(ax_reg != 0x004F)
  {
    printf("no VESA #1\n");
    return 1;
  }

  if(memcmp(VIB.VbeSignature, "VESA", 4) != 0)
  {
    printf("no VESA #2\n");
    return 1;
  }

  oem_str = to_str(&VIB.OemStringPtr);
  
  printf("  Signature: %.4s\n", VIB.VbeSignature);
  printf("  VbeVersion: 0x%04X\n", VIB.VbeVersion);
  printf("  OemStringPtr: %Fs\n", oem_str);
  printf("  Capabilities: 0x%08X\n", VIB.Capabilities);
  printf("  VideoModePtr: 0x%08X\n", VIB.VideoModePtr);
  printf("  TotalMemory: %4d KB\n", VIB.TotalMemory*64);
  printf("---VBE2---\n");
  printf("  OemSoftwareRev: 0x%04X\n", VIB.OemSoftwareRev);
  printf("  OemVendorNamePtr: %s\n", to_str(&VIB.OemVendorNamePtr));
  printf("  OemProductNamePtr: %s\n", to_str(&VIB.OemProductNamePtr));
  printf("  OemProductRevPtr: %s\n", to_str(&VIB.OemProductRevPtr));

  if( _fstrncmp(oem_str, starts_with, strlen(starts_with)) == 0 )
  {
    printf("Found UniVBE #1\n");
  
    printf("int 10h/ax=4F0Fh\n");
    inregs.x.ax = 0x4F0F;
    inregs.x.bx = 0x1234; // gets handled in UniVBE int10h handler someway
    
    ax_reg = int86x(0x10, &inregs, &outregs, &sregs);    
    printf("ax_reg: 0x%04X\n", ax_reg);
    printf("bx_reg: 0x%04X\n", outregs.x.bx);
    printf("cx_reg: 0x%04X\n", outregs.x.cx); // its just the value from the last inregs.x.cx set
    printf("dx_reg: 0x%04X\n", outregs.x.dx);

    if(ax_reg == 0x004F)
    {
      printf("Found UniVBE #2\n");
      
      drv_ofs = outregs.x.bx;
      drv_seg = outregs.x.dx;
      
      if( drv_ofs || drv_seg )
      {
        printf("Found UniVBE #3\n");

        printf("UniVBE int 10h/ax=4F0Fh returns 0x%04X:0x%04X\n", drv_seg, drv_ofs);

        printf("Info from installed TSR (%i bytes)\n------------------------------\n", sizeof(*univbe_data));
        {        
          // details
          univbe_data = MK_FP(drv_seg, drv_ofs);

          printf("Video Card: %Fs\n", univbe_data->card_info);
          printf("RAM DAC: %Fs\n", univbe_data->dac_info);
          printf("Clock Chip: %Fs\n", univbe_data->clock_info);

          //printf("ciphered_registered_user\n");
          //DumpHex(univbe_data->ciphered_registered_user, sizeof(univbe_data->ciphered_registered_user));
          
          _fstrcpy(buffer, univbe_data->ciphered_registered_user);
          deciper_text(buffer);
          printf("deciphered registered user: %s\n", buffer);
          
          //printf("ciphered_serial_no\n");
          //DumpHex(univbe_data->ciphered_serial_no, sizeof(univbe_data->ciphered_serial_no));

          _fstrcpy(buffer, univbe_data->ciphered_serial_no);
          deciper_text(buffer);
          printf("deciphered serial no: %s\n", buffer);

          
          printf("version_major_minor: %.2Fs\n", univbe_data->version_major_minor);
          printf("release_date: %Fs\n", univbe_data->release_date);
          printf("version/license type: 0x%04X\n", univbe_data->version);
          printf("unknown2: 0x%04X ???\n", univbe_data->unknown2);
          
          printf("enabled: 0x%04X (%s)\n", univbe_data->enabled, on_off_str(univbe_data->vbe2_support));
          
          printf("vbe2_support: 0x%04X (%s)\n", univbe_data->vbe2_support, on_off_str(univbe_data->vbe2_support));
          printf("native_mode_centering: 0x%04X (%s)\n", univbe_data->native_mode_centering, on_off_str(univbe_data->native_mode_centering));
          printf("lfb_support: 0x%04X (%s)\n", univbe_data->lfb_support, on_off_str(univbe_data->lfb_support));
          printf("vbe2_palette_functions: 0x%04X (%s)\n", univbe_data->vbe2_palette_functions, on_off_str(univbe_data->vbe2_palette_functions));
          printf("vbe2_32bit_interface: 0x%04X (%s)\n", univbe_data->vbe2_32bit_interface, on_off_str(univbe_data->vbe2_32bit_interface));
          printf("directx2_compatiblity_mode: 0x%04X (%s)\n", univbe_data->directx2_compatiblity_mode, on_off_str(univbe_data->directx2_compatiblity_mode));
        }
        
        {
          memset(&inregs, 0, sizeof(inregs));
          memset(&outregs, 0, sizeof(outregs));
          memset(&sregs, 0, sizeof(sregs));
          inregs.x.ax = 0x3510; // dos int 21h/35h - get vector of int 10h
     
          ax_reg = int86x(0x21, &inregs, &outregs, &sregs);
         
          printf("int 10h vector: es:bx = 0x%04X:0x%04X\n", sregs.es, outregs.x.bx);
          vec10h.segment = sregs.es;
          vec10h.offset = outregs.x.bx;
          
          // at offset 0 is the Signature, at vec10h.offset is the code
          signature = (uint8_t far*)MK_FP(vec10h.segment, 0/*vec10h.offset*/);
          if(_fstrncmp(signature, SIGNATURE, 18) == 0)
          {
            printf("Found UniVBE #4: Signature found at int10h.segment(0x%04X):offset(0)!\n", vec10h.segment);  
          }
        }
        
        {
          memset(&inregs, 0, sizeof(inregs));
          memset(&outregs, 0, sizeof(outregs));
          memset(&sregs, 0, sizeof(sregs));
          inregs.x.ax = 0x352F; // dos int 21h/35h - get vector of int 2Fh
     
          ax_reg = int86x(0x21, &inregs, &outregs, &sregs);
         
          printf("int 2Fh vector: es:bx = 0x%04X:0x%04X\n", sregs.es, outregs.x.bx);
          vec2fh.segment = sregs.es;
          vec2fh.offset = outregs.x.bx;
          
          // UniVBE uinstall-test: uninstall possible if 2F-Int and 10h-Int segment is same
          if( vec10h.segment == vec2fh.segment )
          {
            printf("Found UniVBE #5: Is uninstallable (int10h.segment == int2Fh.segment)\n");
          }
        }
                
        return 0;
      }
    }    
  }
  printf("UniVBE is not installed\n");

  return 0;
}
