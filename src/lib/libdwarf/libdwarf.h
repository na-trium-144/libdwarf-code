/*
  Copyright (C) 2000-2010 Silicon Graphics, Inc.  All Rights Reserved.
  Portions Copyright 2007-2010 Sun Microsystems, Inc. All rights reserved.
  Portions Copyright 2008-2022 David Anderson. All rights reserved.
  Portions Copyright 2008-2010 Arxan Technologies, Inc. All rights reserved.
  Portions Copyright 2010-2012 SN Systems Ltd. All rights reserved.

  This program is free software; you can redistribute it
  and/or modify it under the terms of version 2.1 of the
  GNU Lesser General Public License as published by the Free
  Software Foundation.

  This program is distributed in the hope that it would be
  useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

  Further, this software is distributed without any warranty
  that it is free of the rightful claim of any third person
  regarding infringement or the like.  Any license provided
  herein, whether implied or otherwise, applies only to this
  software file.  Patent licenses, if any, provided herein
  do not apply to combinations of this program with other
  software, or any other product whatsoever.

  You should have received a copy of the GNU Lesser General
  Public License along with this program; if not, write the
  Free Software Foundation, Inc., 51 Franklin Street - Fifth
  Floor, Boston MA 02110-1301, USA.

*/

#ifndef _LIBDWARF_H
#define _LIBDWARF_H

#ifdef DW_API
#undef DW_API
#endif /* DW_API */

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef LIBDWARF_BUILD
#define DW_API __declspec(dllexport)
#else
#define DW_API __declspec(dllimport)
#endif /* LIBDWARF_BUILD */
#elif (defined(__SUNPRO_C)  || defined(__SUNPRO_CC))
#ifdef PIC
#define DW_API __global
#else
#define DW_API
#endif /* PIC */
#elif (defined(__GNUC__) && __GNUC__ >= 4) || \
    defined(__INTEL_COMPILER)
#ifdef PIC
#define DW_API __attribute__ ((visibility("default")))
#else
#define DW_API
#endif /* PIC */
#else
#define DW_API
#endif /* _WIN32 || __CYGWIN__ */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! @file */
/*! @page libdwarf.h
    @tableofcontents
*/
/*
    libdwarf.h
    $Revision: #9 $ $Date: 2008/01/17 $

    For libdwarf consumers (reading DWARF2 and later)

    The interface is defined as having 8-byte signed and unsigned
    values so it can handle 64-or-32bit target on 64-or-32bit host.
    Dwarf_Ptr is the native size: it represents pointers on
    the host machine (not the target!).

    This contains declarations for types and all producer
    and consumer functions.

    Function declarations are written on a single line each here
    so one can use grep  to each declaration in its entirety.
    The declarations are a little harder to read this way, but...
*/
/*! @section defines  Defines and Types
*/

/* Sematic Version identity for this libdwarf.h */
#define DW_LIBDWARF_VERSION "0.3.3"
#define DW_LIBDWARF_VERSION_MAJOR 0
#define DW_LIBDWARF_VERSION_MINOR 3
#define DW_LIBDWARF_VERSION_MICRO 3

#define DW_PATHSOURCE_unspecified 0
#define DW_PATHSOURCE_basic     1
#define DW_PATHSOURCE_dsym      2 /* MacOS dSYM */
#define DW_PATHSOURCE_debuglink 3 /* GNU debuglink */

#ifndef DW_FTYPE_UNKNOWN
#define DW_FTYPE_UNKNOWN    0
#define DW_FTYPE_ELF        1  /* Unix/Linux/etc */
#define DW_FTYPE_MACH_O     2  /* MacOS. */
#define DW_FTYPE_PE         3  /* Windows */
#define DW_FTYPE_ARCHIVE    4  /* unix archive */
#endif /* DW_FTYPE_UNKNOWN */
/* standard return values for functions */
#define DW_DLV_NO_ENTRY -1
#define DW_DLV_OK        0
#define DW_DLV_ERROR     1
/*  These support opening DWARF5 split dwarf objects and
    Elf SHT_GROUP blocks of DWARF sections. */
#define DW_GROUPNUMBER_ANY  0
#define DW_GROUPNUMBER_BASE 1
#define DW_GROUPNUMBER_DWO  2

/*  Special values for offset_into_exception_table field
    of dwarf fde's. */
/*  The following value indicates that there is no
    Exception table offset
    associated with a dwarf frame. */
#define DW_DLX_NO_EH_OFFSET         (-1LL)
/*  The following value indicates that the producer
    was unable to analyse the
    source file to generate Exception tables for this function. */
#define DW_DLX_EH_OFFSET_UNAVAILABLE  (-2LL)

/* The augmenter string for CIE */
#define DW_CIE_AUGMENTER_STRING_V0  "z"

/*  FRAME special values */
/*  The following 3 are assigned numbers, but
    are only present at run time.
    Must not conflict with DW_FRAME values in dwarf.h */
/*  Taken as meaning 'undefined value', this is not
    a column or register number.  */
#ifndef DW_FRAME_UNDEFINED_VAL
#define DW_FRAME_UNDEFINED_VAL          12288
#endif
/*  Taken as meaning 'same value' as caller had, 
    not a column or register number */
#ifndef DW_FRAME_SAME_VAL
#define DW_FRAME_SAME_VAL               12289
#endif
/*  DW_FRAME_CFA_COL is assigned a virtual table position
    but is accessed via CFA specific calls.  */
#ifndef DW_FRAME_CFA_COL
#define DW_FRAME_CFA_COL                12290
#endif
#define DW_FRAME_CFA_COL3 DW_FRAME_CFA_COL /*compatibiliity name*/
/*  END FRAME special values */

/* dwarf_pcline function, slide arguments
*/
#define DW_DLS_BACKWARD   -1  /* slide backward to find line */
#define DW_DLS_NOSLIDE     0  /* match exactly without sliding */
#define DW_DLS_FORWARD     1  /* slide forward to find line */

/*  Defined larger than necessary.
    struct Dwarf_Debug_Fission_Per_CU_s,
    being visible, will be difficult to change:
    binary compatibility. The count is for arrays
    inside the struct, the struct itself is
    a single struct.  */
#define DW_FISSION_SECT_COUNT 12

/*! @defgroup basicdatatypes Basic Library Datatypes Group
    @{
    @typedef Dwarf_Unsigned
    The basic unsigned data type.
    Intended to be an unsigned 64bit value.
    @typedef Dwarf_Signed
    The basic signed data type.
    Intended to be a signed 64bit value.
    @typedef Dwarf_Off
    Used for offsets. It should be same size as Dwarf_Unsigned.
    @typedef Dwarf_Addr
    Used when a data item is a an address represented
    in DWARF. 64 bits. Must be as large as the largest
    object address size.
    @typedef Dwarf_Half
    Many libdwarf values (attribute codes, for example)
    are defined by the standard to be 16 bits, and
    this datatype reflects that (the type must be
    at least 16 bits wide).
    @typedef Dwarf_Bool
    A TRUE(non-zero)/FALSE(zero) data item.
    @typedef Dwarf_Ptr
    A generic pointer type. It uses void *
    so it cannot be added-to or subtracted-from.
    @typedef Dwarf_Small
    Used for small unsigned integers and
    used as Dwarf_Small* for pointers and
    it supports pointer addition and subtraction
    conveniently.
*/
typedef unsigned long long Dwarf_Unsigned;
typedef signed   long long Dwarf_Signed;
typedef unsigned long long Dwarf_Off;
typedef unsigned long long Dwarf_Addr;
    /*  Dwarf_Bool as int is wasteful, but for compatibility
        it must stay as int, not unsigned char. */
typedef int                Dwarf_Bool;   /* boolean type */
typedef unsigned short     Dwarf_Half;   /* 2 byte unsigned value */
typedef unsigned char      Dwarf_Small;  /* 1 byte unsigned value */
/*  If sizeof(Dwarf_Half) is greater than 2
    we believe libdwarf still works properly. */

typedef void*        Dwarf_Ptr;          /* host machine pointer */
/*! @}   endgroup basicdatatypes */

/*! @defgroup enums Enumerators
    @{
    @enum Dwarf_Ranges_Entry_Type
    The dwr_addr1/addr2 data is either an offset (DW_RANGES_ENTRY)
    or an address (dwr_addr2 in DW_RANGES_ADDRESS_SELECTION) or
    both are zero (DW_RANGES_END).
    For DWARF5 each table starts with a header
    followed by range list entries defined
    as here.
    Dwarf_Ranges* apply to DWARF2,3, and 4.
    Not to DWARF5 (the data is different and
    in a new DWARF5 section).
*/
enum Dwarf_Ranges_Entry_Type { DW_RANGES_ENTRY,
    DW_RANGES_ADDRESS_SELECTION,
    DW_RANGES_END
};

/*! @enum Dwarf_Form_Class
    The dwarf specification separates FORMs into
    different classes.  To do the separation properly
    requires 4 pieces of data as of DWARF4 (thus the
    function arguments listed here).
    The DWARF4 specification class definition suffices to
    describe all DWARF versions.
    See section 7.5.4, Attribute Encodings.
    A return of DW_FORM_CLASS_UNKNOWN means we
    could not properly figure
    out what form-class it is.

    DW_FORM_CLASS_FRAMEPTR is MIPS/IRIX only, and refers
    to the DW_AT_MIPS_fde attribute (a reference to the
    .debug_frame section).

    DWARF5:
    DW_FORM_CLASS_LOCLISTSPTR  is like DW_FORM_CLASS_LOCLIST
    except that LOCLISTSPTR is aways a section offset,
    never an index, and LOCLISTSPTR is only referenced
    by DW_AT_loclists_base.
    Note DW_FORM_CLASS_LOCLISTSPTR spelling to distinguish
    from DW_FORM_CLASS_LOCLISTPTR.

    DWARF5:
    DW_FORM_CLASS_RNGLISTSPTR  is like DW_FORM_CLASS_RNGLIST
    except that RNGLISTSPTR is aways a section offset,
    never an index. DW_FORM_CLASS_RNGLISTSPTR is only
    referenced by DW_AT_rnglists_base.
*/
enum Dwarf_Form_Class {
    DW_FORM_CLASS_UNKNOWN = 0,
    DW_FORM_CLASS_ADDRESS = 1,
    DW_FORM_CLASS_BLOCK   = 2,
    DW_FORM_CLASS_CONSTANT =3,
    DW_FORM_CLASS_EXPRLOC = 4,
    DW_FORM_CLASS_FLAG    = 5,
    DW_FORM_CLASS_LINEPTR = 6,
    DW_FORM_CLASS_LOCLISTPTR=7,    /* DWARF2,3,4 only */
    DW_FORM_CLASS_MACPTR  = 8,     /* DWARF2,3,4 only */
    DW_FORM_CLASS_RANGELISTPTR=9,  /* DWARF2,3,4 only */
    DW_FORM_CLASS_REFERENCE=10,
    DW_FORM_CLASS_STRING  = 11,
    DW_FORM_CLASS_FRAMEPTR= 12,  /* MIPS/IRIX DWARF2 only */
    DW_FORM_CLASS_MACROPTR= 13,    /* DWARF5 */
    DW_FORM_CLASS_ADDRPTR = 14,    /* DWARF5 */
    DW_FORM_CLASS_LOCLIST = 15,    /* DWARF5 */
    DW_FORM_CLASS_LOCLISTSPTR=16,  /* DWARF5 */
    DW_FORM_CLASS_RNGLIST    =17,  /* DWARF5 */
    DW_FORM_CLASS_RNGLISTSPTR=18,  /* DWARF5 */
    DW_FORM_CLASS_STROFFSETSPTR=19 /* DWARF5 */
};
/*! @}   endgroupenums*/

/*! @defgroup allstructs Defined and Opaque Structs Group
    @{

    @typedef Dwarf_Form_Data16
    a container for a DW_FORM_data16 data item.
    We have no integer types suitable so this special
    struct is used instead.  It is up to consumers/producers
    to deal with the contents.
*/
typedef struct Dwarf_Form_Data16_s {
    unsigned char fd_data[16];
} Dwarf_Form_Data16;

/*! @typedef Dwarf_Sig8
    Used for signatures where ever they appear.
    It is not a string, it
    is 8 bytes of a signature one would use to find
    a type unit.
    @see dwarf_formsig8
*/
typedef struct Dwarf_Sig8_s  {
    char signature[8];
} Dwarf_Sig8;

/*! @typedef Dwarf_Block

    Used to hold uninterpreted blocks of data.
    bl_data refers to on an uninterpreted block of data
    Used with certain location information functions,
    a frame expression function, expanded
    frame instructions, and
    DW_FORM_block functions.

    @see dwarf_formblock
    @see explainformblock

*/
typedef struct Dwarf_Block_s {
    Dwarf_Unsigned  bl_len;
    Dwarf_Ptr       bl_data;
    Dwarf_Small     bl_from_loclist;
    Dwarf_Unsigned  bl_section_offset;
} Dwarf_Block;

/*! @typedef Dwarf_Locdesc_c
    Provides access to Dwarf_Locdesc_c, a single
    location description
*/
typedef struct Dwarf_Locdesc_c_s * Dwarf_Locdesc_c;

/*! @typedef Dwarf_Loc_Head_c
    provides access to any sort of location description
    for DWARF2,3,4, or 5.
*/
typedef struct Dwarf_Loc_Head_c_s * Dwarf_Loc_Head_c;

/*  This provides access to data from sections
    .debug_gnu_pubtypes or .debug_gnu_pubnames.
    These are not standard DWARF, and can appear
    with gcc -gdwarf-5
    */
typedef struct Dwarf_Gnu_Index_Head_s * Dwarf_Gnu_Index_Head;

/*! @typedef Dwarf_Dsc_Head
    Access to DW_AT_discr_list
    array of discriminant values.
*/
typedef struct Dwarf_Dsc_Head_s * Dwarf_Dsc_Head;

/*! @typedef Dwarf_Frame_Instr_Head
    The basis for access to DWARF frame instructions
    (FDE or CIE)
    in full detail.
*/
typedef struct Dwarf_Frame_Instr_Head_s * Dwarf_Frame_Instr_Head;

/*! @typedef dwarf_printf_callback_function_type

    Used as a function pointer to a user-written
    callback function.
*/
typedef void (* dwarf_printf_callback_function_type)
    (void * /*user_pointer*/, const char * /*linecontent*/);

struct Dwarf_Printf_Callback_Info_s {
    void *                        dp_user_pointer;
    dwarf_printf_callback_function_type dp_fptr;
    char *                        dp_buffer;
    unsigned int                  dp_buffer_len;
    int                           dp_buffer_user_provided;
    void *                        dp_reserved;
};

/*! @typedef Dwarf_Cmdline_Options.

    check_verbose_mode defaults to FALSE.
    If a libdwarf-calling program sets
    this TRUE it means some errors in Line Table
    headers get a much more detailed description
    of the error which is reported the caller via
    printf_callback() function (the caller
    can do something with the message).
    Or the libdwarf calling code can call
    dwarf_record_cmdline_options() to set
    the new value.
*/
typedef struct Dwarf_Cmdline_Options_s {
    Dwarf_Bool check_verbose_mode;
} Dwarf_Cmdline_Options;

/*! @typedef Dwarf_Str_Offsets_Table
    Provides an access to the .debug_str_offsets
    section indepenently of other DWARF sections.
    Mainly of use in examining the .debug_str_offsets
    section content for problems.
*/
typedef struct Dwarf_Str_Offsets_Table_s *  Dwarf_Str_Offsets_Table;

/*! @typedef Dwarf_Ranges
    Details of of non-contiguous address ranges
    of DIEs for DWARF2, DWARF3, and DWARF4.
    Sufficient for older dwarf.
*/
typedef struct Dwarf_Ranges_s {
    Dwarf_Addr dwr_addr1;
    Dwarf_Addr dwr_addr2;
    enum Dwarf_Ranges_Entry_Type  dwr_type;
} Dwarf_Ranges;

/*! @typedef Dwarf_Regtable_Entry3
    For each index i (naming a hardware register with dwarf number
    i) the following is true and defines the value of that register:

        If dw_regnum is Register DW_FRAME_UNDEFINED_VAL
            it is not DWARF register number but
            a place holder indicating the register
            has no defined value.
        If dw_regnum is Register DW_FRAME_SAME_VAL
            it  is not DWARF register number but
            a place holder indicating the register has the same
            value in the previous frame.

            DW_FRAME_UNDEFINED_VAL, DW_FRAME_SAME_VAL and
            DW_FRAME_CFA_COL are only present at libdwarf runtime.
            Never on disk.
            DW_FRAME_* Values present on disk are in dwarf.h
            Because DW_FRAME_SAME_VAL and DW_FRAME_UNDEFINED_VAL
            and DW_FRAME_CFA_COL are definable at runtime
            consider the names symbolic in this comment,
            not absolute.

        Otherwise: the register number is a DWARF register number
            (see ABI documents for how this translates to hardware/
            software register numbers in the machine hardware)
            and the following applies:

        In a cfa-defining entry (rt3_cfa_rule) the regnum is the
        CFA 'register number'. Which is some 'normal' register,
        not DW_FRAME_CFA_COL, nor DW_FRAME_SAME_VAL, nor
        DW_FRAME_UNDEFINED_VAL.

        If dw_value_type == DW_EXPR_OFFSET (the only
        possible case for dwarf2):
            If dw_offset_relevant is non-zero, then
                the value is stored at at the address
                CFA+N where N is a signed offset.
                dw_regnum is the cfa register rule which means
                one ignores dw_regnum and uses the CFA appropriately.
                So dw_offset is a signed value, really,
                and must be printed/evaluated as such.
                Rule: Offset(N)
            If dw_offset_relevant is zero, then the
                value of the register
                is the value of (DWARF) register number dw_regnum.
                Rule: register(R)
        If dw_value_type  == DW_EXPR_VAL_OFFSET
            the  value of this register is CFA +N where
            N is a signed offset.
            dw_regnum is the cfa register rule which means
            one ignores dw_regnum and uses the CFA appropriately.
            Rule: val_offset(N)
        If dw_value_type  == DW_EXPR_EXPRESSION
            The value of the register is the value at the address
            computed by evaluating the DWARF expression E.
            Rule: expression(E)
            The expression E byte stream is pointed to by
            block.bl_data.
            The expression length in bytes is given by
            block.bl_len.
        If dw_value_type  == DW_EXPR_VAL_EXPRESSION
            The value of the register is the value
            computed by evaluating the DWARF expression E.
            Rule: val_expression(E)
            The expression E byte stream is pointed to
            by block.bl_data.
            The expression length in bytes is given by
            block.bl_len.
        Other values of dw_value_type are an error.
*/

typedef struct Dwarf_Regtable_Entry3_s {
    Dwarf_Small         dw_offset_relevant;
    Dwarf_Small         dw_value_type;
    Dwarf_Half          dw_regnum;
    Dwarf_Unsigned      dw_offset;
    Dwarf_Unsigned      dw_args_size; /* Not dealt with.  */
    Dwarf_Block         dw_block;
} Dwarf_Regtable_Entry3;

/*! @typedef  Dwarf_Regtable3
    This structs provides a way for applications
    to select the number of frame registers
    and to select names for them.

    rt3_rules and rt3_reg_table_size must be filled in before
    calling libdwarf.  Filled in with a pointer to an array
    (pointer and array  set up by the calling application)
    of rt3_reg_table_size Dwarf_Regtable_Entry3_s structs.
    libdwarf does not allocate or deallocate space for the
    rules, you must do so.   libdwarf will initialize the
    contents rules array, you do not need to do so (though
    if you choose to initialize the array somehow that is ok:
    libdwarf will overwrite your initializations with its own).
*/
typedef struct Dwarf_Regtable3_s {
    struct Dwarf_Regtable_Entry3_s   rt3_cfa_rule;
    Dwarf_Half                       rt3_reg_table_size;
    struct Dwarf_Regtable_Entry3_s * rt3_rules;
} Dwarf_Regtable3;

/* Opaque types for Consumer Library. */
/*! @typedef Dwarf_Error
    &error is used in most calls to return error details
    when the call returns DW_DLV_ERROR.
*/
typedef struct Dwarf_Error_s*      Dwarf_Error;
/*! @typedef Dwarf_Debug
    An open Dwarf_Debug points to data that libdwarf
    maintains to support libdwarf calls.
*/
typedef struct Dwarf_Debug_s*      Dwarf_Debug;
typedef struct Dwarf_Die_s*        Dwarf_Die;
typedef struct Dwarf_Line_s*       Dwarf_Line;
typedef struct Dwarf_Global_s*     Dwarf_Global;
typedef struct Dwarf_Type_s*       Dwarf_Type;
/* The next three are SGI extensions not used elsewhere. */
typedef struct Dwarf_Func_s*       Dwarf_Func;
typedef struct Dwarf_Var_s*        Dwarf_Var;
typedef struct Dwarf_Weak_s*       Dwarf_Weak;
typedef struct Dwarf_Error_s*      Dwarf_Error;
typedef struct Dwarf_Attribute_s*  Dwarf_Attribute;
typedef struct Dwarf_Abbrev_s*     Dwarf_Abbrev;
typedef struct Dwarf_Fde_s*        Dwarf_Fde;
typedef struct Dwarf_Cie_s*        Dwarf_Cie;
typedef struct Dwarf_Arange_s*     Dwarf_Arange;
typedef struct Dwarf_Gdbindex_s*   Dwarf_Gdbindex;
typedef struct Dwarf_Xu_Index_Header_s  *Dwarf_Xu_Index_Header;
typedef struct Dwarf_Line_Context_s     *Dwarf_Line_Context;
typedef struct Dwarf_Macro_Context_s    *Dwarf_Macro_Context;
typedef struct Dwarf_Dnames_Head_s      *Dwarf_Dnames_Head;

/*! @typedef Dwarf_Handler
    Used in rare cases (mainly tiny programs)
    with dwarf_init_path() etc
    initialization calls.
*/
typedef void  (*Dwarf_Handler)(Dwarf_Error dw_error,
    Dwarf_Ptr dw_errarg);

/*! @struct Dwarf_Macro_Details_s

    This applies to DWARF2, DWARF3, and DWARF4
    compilation units.
    DWARF5 .debug_macro has its own function interface
    which does not use this struct.
*/
struct Dwarf_Macro_Details_s {
    Dwarf_Off    dmd_offset; /* offset, in the section,
        of this macro info */
    Dwarf_Small  dmd_type;   /* the type, DW_MACINFO_define etc*/
    Dwarf_Signed dmd_lineno; /* the source line number where
        applicable and vend_def number if
        vendor_extension op */

    Dwarf_Signed dmd_fileindex;/* the source file index:
        applies to define undef start_file */
    char *       dmd_macro;  /* macro name (with value for defineop)
        string from vendor ext */
};
/*! @typedef Dwarf_Rnglists_Head
    Used for access to a set of DWARF5 debug_rnglists
    entries.
*/
typedef struct Dwarf_Rnglists_Head_s * Dwarf_Rnglists_Head;
typedef struct Dwarf_Obj_Access_Interface_a_s
    Dwarf_Obj_Access_Interface_a;
typedef struct Dwarf_Obj_Access_Methods_a_s
    Dwarf_Obj_Access_Methods_a;
typedef struct Dwarf_Obj_Access_Section_a_s
    Dwarf_Obj_Access_Section_a;
typedef struct Dwarf_Macro_Details_s Dwarf_Macro_Details;
typedef struct Dwarf_Debug_Fission_Per_CU_s
    Dwarf_Debug_Fission_Per_CU;

/*  Used in the get_section interface function
    in Dwarf_Obj_Access_Section_a_s.  Since libdwarf
    depends on standard DWARF section names an object
    format that has no such names (but has some
    method of setting up 'sections equivalents')
    must arrange to return standard DWARF section
    names in the 'name' field.  libdwarf does
    not free the strings in 'name'.
    For non-elf many of the fields should be zero.*/
struct Dwarf_Obj_Access_Section_a_s {
    /*  Having an accurate section name makes
        debugging of libdwarf easier.
        and is essential to find the .debug_ sections.  */
    const char*    as_name;
    Dwarf_Unsigned as_type;
    Dwarf_Unsigned as_flags;
    /*  addr is the virtual address of the first byte of
        the section data.  Usually zero when the address
        makes no sense for a given section. */
    Dwarf_Addr     as_addr;
    Dwarf_Unsigned as_offset; /* file offset of section */

    /* Size in bytes of the section. */
    Dwarf_Unsigned as_size;

    /*  Set link to zero if it is meaningless.  If non-zero
        it should be a link to a rela section or from symtab
        to strtab.  In Elf it is sh_link. */
    Dwarf_Unsigned as_link;

    /*  The section header index of the section to which the
        relocation applies. In Elf it is sh_info. */
    Dwarf_Unsigned as_info;

    Dwarf_Unsigned as_addralign;
    /*  Elf sections that are tables have a non-zero entrysize so
        the count of entries can be calculated even without
        the right structure definition. If your object format
        does not have this data leave this zero. */
    Dwarf_Unsigned as_entrysize;
};

/*! @struct Dwarf_Obj_Access_Methods_a_s:
    The functions we need to access object data
    from libdwarf are declared here.

    In these function pointer declarations
    'void *obj' is intended to be a pointer (the object field in
    Dwarf_Obj_Access_Interface_s)
    that hides the library-specific and object-specific
    data that makes
    it possible to handle multiple object formats
    and multiple libraries.
    It's not required that one handles multiple such
    in a single libdwarf
    archive/shared-library (but not ruled out either).
    See  dwarf_elf_object_access_internals_t and dwarf_elf_access.c
    for an example.

    Usually the struct is statically defined
    and the function pointers are set at
    compile time.

    The om_get_filesize member is new September 4, 2021.
    Its postion is NOT at the end of the list.
    The member names all now have om_ prefix.
*/
/*
    om_get_section_info

    Get address, size, and name info about a section.

    Parameters
    section_index - Zero-based index.
    return_section - Pointer to a structure in which
        section info will be placed.   Caller must
        provide a valid pointer to a
        structure area.  The structure's contents
        will be overwritten
        by the call to get_section_info.
    error - A pointer to an integer in which an error
        code may be stored.

    Return
    DW_DLV_OK - Everything ok.
    DW_DLV_ERROR - Error occurred. Use 'error' to determine the
        libdwarf defined error.
    DW_DLV_NO_ENTRY - No such section.  */
/*
    om_get_byte_order

    Get whether the object file represented by
    this interface is big-endian
    (DW_END_big) or little endian (DW_END_little).

    Parameters
    obj - Equivalent to 'this' in OO languages.

    Return
    Endianness of object. Cannot fail.  */
/*
    om_get_length_size

    Get the size of a length field in the underlying object file.
    libdwarf currently supports * 4 and 8 byte sizes, but may
    support larger in the future.
    Perhaps the return type should be an enumeration?

    Parameters
    obj - Equivalent to 'this' in OO languages.

    Return
    Size of length. Cannot fail.  */
/*
    om_get_pointer_size

    Get the size of a pointer field in the underlying object file.
    libdwarf currently supports  4 and 8 byte sizes.
    Perhaps the return type should be an enumeration?

    Return
    Size of pointer. Cannot fail.  */
/*
    om_get_filesize

    Returns a value that is a sanity check on
    offsets libdwarf reads.  Must be larger than any section size
    libdwarf might read.  It need not be a tight bound.
    In dwarf_init_path() etc libdwarf uses the object file
    size as the value.

    Return
    A value at least as large as any section libdwarf
    might read.  */
/*
    om_get_section_count

    Get the number of sections in the object file.

    Parameters

    Return
    Number of sections */
/*
    om_load_section

    Get a pointer to an array of bytes that
    represent the section.

    Parameters
    section_index - Zero-based index.
    return_data - The address of a pointer to
        which the section data block
        will be assigned.
    error - Pointer to an integer for returning
        libdwarf-defined error numbers.

    Return
    DW_DLV_OK - No error.
    DW_DLV_ERROR - Error. Use 'error' to indicate
        a libdwarf-defined error number.
    DW_DLV_NO_ENTRY - No such section.  */
/*
    om_relocate_a_section
    If relocations are not supported leave this pointer NULL.

    Get a pointer to an array of bytes that represent
    the section.

    Parameters
    section_index - Zero-based index of the
        section to be relocated.
    error - Pointer to an integer for returning libdwarf-defined
        error numbers.

    Return
    DW_DLV_OK - No error.
    DW_DLV_ERROR - Error. Use 'error' to indicate
        a libdwarf-defined
        error number.
    DW_DLV_NO_ENTRY - No such section.  */
struct Dwarf_Obj_Access_Methods_a_s {
    int    (*om_get_section_info)(void* obj,
        Dwarf_Half section_index,
        Dwarf_Obj_Access_Section_a* return_section,
        int* error);
    Dwarf_Small      (*om_get_byte_order)(void* obj);
    Dwarf_Small      (*om_get_length_size)(void* obj);
    Dwarf_Small      (*om_get_pointer_size)(void* obj);
    Dwarf_Unsigned   (*om_get_filesize)(void* obj);

    Dwarf_Unsigned   (*om_get_section_count)(void* obj);
    int              (*om_load_section)(void* obj,
        Dwarf_Half section_index,
        Dwarf_Small** return_data, int* error);
    int              (*om_relocate_a_section)(void* obj,
        Dwarf_Half section_index,
        Dwarf_Debug dbg,
        int* error);
};

/*  struct Dwarf_Obj_Access_Interface_a_s is allocated
    and deallocated by your code when you are using
    the libdwarf Object File Interface
    [dwarf_object_init_b() directly.
*/
struct Dwarf_Obj_Access_Interface_a_s {
    void*                             ai_object;
    const Dwarf_Obj_Access_Methods_a *ai_methods;
};

/*  User code must allocate this struct, zero it,
    and pass a pointer to it
    into dwarf_get_debugfission_for_cu .  */
struct Dwarf_Debug_Fission_Per_CU_s  {
    /*  Do not free the string. It contains "cu" or "tu". */
    /*  If this is not set (ie, not a CU/TU in  DWP Package File)
        then pcu_type will be NULL.  */
    const char   * pcu_type;
    /*  pcu_index is the index (range 1 to N )
        into the tu/cu table of offsets and the table
        of sizes.  1 to N as the zero index is reserved
        for special purposes.  Not a value one
        actually needs. */
    Dwarf_Unsigned pcu_index;
    Dwarf_Sig8     pcu_hash;  /* 8 byte  */
    /*  [0] has offset and size 0.
        [1]-[8] are DW_SECT_* indexes and the
        values are  the offset and size
        of the respective section contribution
        of a single .dwo object. When pcu_size[n] is
        zero the corresponding section is not present. */
    Dwarf_Unsigned pcu_offset[DW_FISSION_SECT_COUNT];
    Dwarf_Unsigned pcu_size[DW_FISSION_SECT_COUNT];
    Dwarf_Unsigned unused1;
    Dwarf_Unsigned unused2;
};

/*! @typedef Dwarf_Rnglists_Head
    Used for access to a set of DWARF5 debug_rnglists
    entries.
*/
typedef struct Dwarf_Rnglists_Head_s * Dwarf_Rnglists_Head;

/*! @} endgroup allstructs */

/*! @defgroup framedefines Default frame #define values
    @{
*/
/*  Special values for offset_into_exception_table field
    of dwarf fde's. */
/*  The following value indicates that there is no
    Exception table offset
    associated with a dwarf frame. */
#define DW_DLX_NO_EH_OFFSET         (-1LL)
/*  The following value indicates that the producer
    was unable to analyse the
    source file to generate Exception tables for this function. */
#define DW_DLX_EH_OFFSET_UNAVAILABLE  (-2LL)

/* The augmenter string for CIE */
#define DW_CIE_AUGMENTER_STRING_V0  "z"

/*  ***IMPORTANT NOTE, TARGET DEPENDENCY ****
    DW_REG_TABLE_SIZE must be at least as large as
    the number of registers
    DW_FRAME_LAST_REG_NUM as defined in dwarf.h
*/
#ifndef DW_REG_TABLE_SIZE
#define DW_REG_TABLE_SIZE  DW_FRAME_LAST_REG_NUM
#endif

/* For MIPS, DW_FRAME_SAME_VAL is the correct default value
   for a frame register value. For other CPUS another value
   may be better, such as DW_FRAME_UNDEFINED_VAL.
   See dwarf_set_frame_rule_table_size
*/
#ifndef DW_FRAME_REG_INITIAL_VALUE
#define DW_FRAME_REG_INITIAL_VALUE DW_FRAME_SAME_VAL
#endif

/* The following are all needed to evaluate DWARF3 register rules.
   These have nothing to do simply printing
   frame instructions.
*/
#define DW_EXPR_OFFSET         0 /* offset is from CFA reg */
#define DW_EXPR_VAL_OFFSET     1
#define DW_EXPR_EXPRESSION     2
#define DW_EXPR_VAL_EXPRESSION 3
/*! @} */

/*! @defgroup dwdla  DW_DLA #define values

    These identify the various allocate/dealloc
    types.  The allocation happens within libdwarf,
    and the deallocation is usually done by
    user code.
    @{
*/
#define DW_DLA_STRING          0x01  /* char* */
#define DW_DLA_LOC             0x02  /* Dwarf_Loc */
#define DW_DLA_LOCDESC         0x03  /* Dwarf_Locdesc */
#define DW_DLA_ELLIST          0x04  /* Dwarf_Ellist (not used)*/
#define DW_DLA_BOUNDS          0x05  /* Dwarf_Bounds (not used) */
#define DW_DLA_BLOCK           0x06  /* Dwarf_Block */
#define DW_DLA_DEBUG           0x07  /* Dwarf_Debug */
#define DW_DLA_DIE             0x08  /* Dwarf_Die */
#define DW_DLA_LINE            0x09  /* Dwarf_Line */
#define DW_DLA_ATTR            0x0a  /* Dwarf_Attribute */
#define DW_DLA_TYPE            0x0b  /* Dwarf_Type  (not used) */
#define DW_DLA_SUBSCR          0x0c  /* Dwarf_Subscr (not used) */
#define DW_DLA_GLOBAL          0x0d  /* Dwarf_Global */
#define DW_DLA_ERROR           0x0e  /* Dwarf_Error */
#define DW_DLA_LIST            0x0f  /* a list */
#define DW_DLA_LINEBUF         0x10  /* Dwarf_Line* (not used) */
#define DW_DLA_ARANGE          0x11  /* Dwarf_Arange */
#define DW_DLA_ABBREV          0x12  /* Dwarf_Abbrev */
#define DW_DLA_FRAME_INSTR_HEAD   0x13  /* Dwarf_Frame_Instr_Head */
#define DW_DLA_CIE             0x14  /* Dwarf_Cie */
#define DW_DLA_FDE             0x15  /* Dwarf_Fde */
#define DW_DLA_LOC_BLOCK       0x16  /* Dwarf_Loc */

#define DW_DLA_FRAME_OP        0x17 /* Dwarf_Frame_Op (not used) */
#define DW_DLA_FUNC            0x18  /* Dwarf_Func */
#define DW_DLA_TYPENAME        0x19  /* Dwarf_Type */
#define DW_DLA_VAR             0x1a  /* Dwarf_Var */
#define DW_DLA_WEAK            0x1b  /* Dwarf_Weak */
#define DW_DLA_ADDR            0x1c  /* Dwarf_Addr sized entries */
#define DW_DLA_RANGES          0x1d  /* Dwarf_Ranges */
/* 0x1e (30) to 0x34 (52) reserved for internal to libdwarf types. */
/* .debug_gnu_typenames/pubnames, 2020 */
#define DW_DLA_GNU_INDEX_HEAD  0x35

#define DW_DLA_RNGLISTS_HEAD   0x36  /* .debug_rnglists DW5 */
#define DW_DLA_GDBINDEX        0x37  /* Dwarf_Gdbindex */
#define DW_DLA_XU_INDEX        0x38  /* Dwarf_Xu_Index_Header */
#define DW_DLA_LOC_BLOCK_C     0x39  /* Dwarf_Loc_c*/
#define DW_DLA_LOCDESC_C       0x3a  /* Dwarf_Locdesc_c */
#define DW_DLA_LOC_HEAD_C      0x3b  /* Dwarf_Loc_Head_c */
#define DW_DLA_MACRO_CONTEXT   0x3c  /* Dwarf_Macro_Context */
/*  0x3d (61) is for libdwarf internal use.               */
#define DW_DLA_DSC_HEAD        0x3e  /* Dwarf_Dsc_Head */
#define DW_DLA_DNAMES_HEAD     0x3f  /* Dwarf_Dnames_Head */

/* struct Dwarf_Str_Offsets_Table_s */
#define DW_DLA_STR_OFFSETS     0x40
/*! @} */

/*! @defgroup dwdle  DW_DLE #define Error Numbers

    These identify the various error codes that have been
    used.  Not all of them are still use.
    We do not recycle obsolete codes into new uses.
    The codes 1 through 22 are historic and it is
    unlikely they are used anywhere in the library.
    @{
*/
/* libdwarf error numbers */
#define DW_DLE_NE      0  /* no error */
#define DW_DLE_VMM     1  /* dwarf format/library version mismatch */
#define DW_DLE_MAP     2  /* memory map failure */
#define DW_DLE_LEE     3  /* libelf error */
#define DW_DLE_NDS     4  /* no debug section */
#define DW_DLE_NLS     5  /* no line section */
#define DW_DLE_ID      6  /* invalid descriptor for query */
#define DW_DLE_IOF     7  /* I/O failure */
#define DW_DLE_MAF     8  /* memory allocation failure */
#define DW_DLE_IA      9  /* invalid argument */
#define DW_DLE_MDE     10 /* mangled debugging entry */
#define DW_DLE_MLE     11 /* mangled line number entry */
#define DW_DLE_FNO     12 /* file not open */
#define DW_DLE_FNR     13 /* file not a regular file */
#define DW_DLE_FWA     14 /* file open with wrong access */
#define DW_DLE_NOB     15 /* not an object file */
#define DW_DLE_MOF     16 /* mangled object file header */
#define DW_DLE_EOLL    17 /* end of location list entries */
#define DW_DLE_NOLL    18 /* no location list section */
#define DW_DLE_BADOFF  19 /* Invalid offset */
#define DW_DLE_EOS     20 /* end of section  */
#define DW_DLE_ATRUNC  21 /* abbreviations section appears truncated*/
#define DW_DLE_BADBITC 22 /* Address size passed to dwarf bad,*/
    /* It is not an allowed size (64 or 32) */
    /* Error codes defined by the current Libdwarf Implementation. */
#define DW_DLE_DBG_ALLOC                        23
#define DW_DLE_FSTAT_ERROR                      24
#define DW_DLE_FSTAT_MODE_ERROR                 25
#define DW_DLE_INIT_ACCESS_WRONG                26
#define DW_DLE_ELF_BEGIN_ERROR                  27
#define DW_DLE_ELF_GETEHDR_ERROR                28
#define DW_DLE_ELF_GETSHDR_ERROR                29
#define DW_DLE_ELF_STRPTR_ERROR                 30
#define DW_DLE_DEBUG_INFO_DUPLICATE             31
#define DW_DLE_DEBUG_INFO_NULL                  32
#define DW_DLE_DEBUG_ABBREV_DUPLICATE           33
#define DW_DLE_DEBUG_ABBREV_NULL                34
#define DW_DLE_DEBUG_ARANGES_DUPLICATE          35
#define DW_DLE_DEBUG_ARANGES_NULL               36
#define DW_DLE_DEBUG_LINE_DUPLICATE             37
#define DW_DLE_DEBUG_LINE_NULL                  38
#define DW_DLE_DEBUG_LOC_DUPLICATE              39
#define DW_DLE_DEBUG_LOC_NULL                   40
#define DW_DLE_DEBUG_MACINFO_DUPLICATE          41
#define DW_DLE_DEBUG_MACINFO_NULL               42
#define DW_DLE_DEBUG_PUBNAMES_DUPLICATE         43
#define DW_DLE_DEBUG_PUBNAMES_NULL              44
#define DW_DLE_DEBUG_STR_DUPLICATE              45
#define DW_DLE_DEBUG_STR_NULL                   46
#define DW_DLE_CU_LENGTH_ERROR                  47
#define DW_DLE_VERSION_STAMP_ERROR              48
#define DW_DLE_ABBREV_OFFSET_ERROR              49
#define DW_DLE_ADDRESS_SIZE_ERROR               50
#define DW_DLE_DEBUG_INFO_PTR_NULL              51
#define DW_DLE_DIE_NULL                         52
#define DW_DLE_STRING_OFFSET_BAD                53
#define DW_DLE_DEBUG_LINE_LENGTH_BAD            54
#define DW_DLE_LINE_PROLOG_LENGTH_BAD           55
#define DW_DLE_LINE_NUM_OPERANDS_BAD            56
#define DW_DLE_LINE_SET_ADDR_ERROR              57
#define DW_DLE_LINE_EXT_OPCODE_BAD              58
#define DW_DLE_DWARF_LINE_NULL                  59
#define DW_DLE_INCL_DIR_NUM_BAD                 60
#define DW_DLE_LINE_FILE_NUM_BAD                61
#define DW_DLE_ALLOC_FAIL                       62
#define DW_DLE_NO_CALLBACK_FUNC                 63
#define DW_DLE_SECT_ALLOC                       64
#define DW_DLE_FILE_ENTRY_ALLOC                 65
#define DW_DLE_LINE_ALLOC                       66
#define DW_DLE_FPGM_ALLOC                       67
#define DW_DLE_INCDIR_ALLOC                     68
#define DW_DLE_STRING_ALLOC                     69
#define DW_DLE_CHUNK_ALLOC                      70
#define DW_DLE_BYTEOFF_ERR                      71
#define DW_DLE_CIE_ALLOC                        72
#define DW_DLE_FDE_ALLOC                        73
#define DW_DLE_REGNO_OVFL                       74
#define DW_DLE_CIE_OFFS_ALLOC                   75
#define DW_DLE_WRONG_ADDRESS                    76
#define DW_DLE_EXTRA_NEIGHBORS                  77
#define DW_DLE_WRONG_TAG                        78
#define DW_DLE_DIE_ALLOC                        79
#define DW_DLE_PARENT_EXISTS                    80
#define DW_DLE_DBG_NULL                         81
#define DW_DLE_DEBUGLINE_ERROR                  82
#define DW_DLE_DEBUGFRAME_ERROR                 83
#define DW_DLE_DEBUGINFO_ERROR                  84
#define DW_DLE_ATTR_ALLOC                       85
#define DW_DLE_ABBREV_ALLOC                     86
#define DW_DLE_OFFSET_UFLW                      87
#define DW_DLE_ELF_SECT_ERR                     88
#define DW_DLE_DEBUG_FRAME_LENGTH_BAD           89
#define DW_DLE_FRAME_VERSION_BAD                90
#define DW_DLE_CIE_RET_ADDR_REG_ERROR           91
#define DW_DLE_FDE_NULL                         92
#define DW_DLE_FDE_DBG_NULL                     93
#define DW_DLE_CIE_NULL                         94
#define DW_DLE_CIE_DBG_NULL                     95
#define DW_DLE_FRAME_TABLE_COL_BAD              96
#define DW_DLE_PC_NOT_IN_FDE_RANGE              97
#define DW_DLE_CIE_INSTR_EXEC_ERROR             98
#define DW_DLE_FRAME_INSTR_EXEC_ERROR           99
#define DW_DLE_FDE_PTR_NULL                    100
#define DW_DLE_RET_OP_LIST_NULL                101
#define DW_DLE_LINE_CONTEXT_NULL               102
#define DW_DLE_DBG_NO_CU_CONTEXT               103
#define DW_DLE_DIE_NO_CU_CONTEXT               104
#define DW_DLE_FIRST_DIE_NOT_CU                105
#define DW_DLE_NEXT_DIE_PTR_NULL               106
#define DW_DLE_DEBUG_FRAME_DUPLICATE           107
#define DW_DLE_DEBUG_FRAME_NULL                108
#define DW_DLE_ABBREV_DECODE_ERROR             109
#define DW_DLE_DWARF_ABBREV_NULL               110
#define DW_DLE_ATTR_NULL                       111
#define DW_DLE_DIE_BAD                         112
#define DW_DLE_DIE_ABBREV_BAD                  113
#define DW_DLE_ATTR_FORM_BAD                   114
#define DW_DLE_ATTR_NO_CU_CONTEXT              115
#define DW_DLE_ATTR_FORM_SIZE_BAD              116
#define DW_DLE_ATTR_DBG_NULL                   117
#define DW_DLE_BAD_REF_FORM                    118
#define DW_DLE_ATTR_FORM_OFFSET_BAD            119
#define DW_DLE_LINE_OFFSET_BAD                 120
#define DW_DLE_DEBUG_STR_OFFSET_BAD            121
#define DW_DLE_STRING_PTR_NULL                 122
#define DW_DLE_PUBNAMES_VERSION_ERROR          123
#define DW_DLE_PUBNAMES_LENGTH_BAD             124
#define DW_DLE_GLOBAL_NULL                     125
#define DW_DLE_GLOBAL_CONTEXT_NULL             126
#define DW_DLE_DIR_INDEX_BAD                   127
#define DW_DLE_LOC_EXPR_BAD                    128
#define DW_DLE_DIE_LOC_EXPR_BAD                129
#define DW_DLE_ADDR_ALLOC                      130
#define DW_DLE_OFFSET_BAD                      131
#define DW_DLE_MAKE_CU_CONTEXT_FAIL            132
#define DW_DLE_REL_ALLOC                       133
#define DW_DLE_ARANGE_OFFSET_BAD               134
#define DW_DLE_SEGMENT_SIZE_BAD                135
#define DW_DLE_ARANGE_LENGTH_BAD               136
#define DW_DLE_ARANGE_DECODE_ERROR             137
#define DW_DLE_ARANGES_NULL                    138
#define DW_DLE_ARANGE_NULL                     139
#define DW_DLE_NO_FILE_NAME                    140
#define DW_DLE_NO_COMP_DIR                     141
#define DW_DLE_CU_ADDRESS_SIZE_BAD             142
#define DW_DLE_INPUT_ATTR_BAD                  143
#define DW_DLE_EXPR_NULL                       144
#define DW_DLE_BAD_EXPR_OPCODE                 145
#define DW_DLE_EXPR_LENGTH_BAD                 146
#define DW_DLE_MULTIPLE_RELOC_IN_EXPR          147
#define DW_DLE_ELF_GETIDENT_ERROR              148
#define DW_DLE_NO_AT_MIPS_FDE                  149
#define DW_DLE_NO_CIE_FOR_FDE                  150
#define DW_DLE_DIE_ABBREV_LIST_NULL            151
#define DW_DLE_DEBUG_FUNCNAMES_DUPLICATE       152
#define DW_DLE_DEBUG_FUNCNAMES_NULL            153
#define DW_DLE_DEBUG_FUNCNAMES_VERSION_ERROR   154
#define DW_DLE_DEBUG_FUNCNAMES_LENGTH_BAD      155
#define DW_DLE_FUNC_NULL                       156
#define DW_DLE_FUNC_CONTEXT_NULL               157
#define DW_DLE_DEBUG_TYPENAMES_DUPLICATE       158
#define DW_DLE_DEBUG_TYPENAMES_NULL            159
#define DW_DLE_DEBUG_TYPENAMES_VERSION_ERROR   160
#define DW_DLE_DEBUG_TYPENAMES_LENGTH_BAD      161
#define DW_DLE_TYPE_NULL                       162
#define DW_DLE_TYPE_CONTEXT_NULL               163
#define DW_DLE_DEBUG_VARNAMES_DUPLICATE        164
#define DW_DLE_DEBUG_VARNAMES_NULL             165
#define DW_DLE_DEBUG_VARNAMES_VERSION_ERROR    166
#define DW_DLE_DEBUG_VARNAMES_LENGTH_BAD       167
#define DW_DLE_VAR_NULL                        168
#define DW_DLE_VAR_CONTEXT_NULL                169
#define DW_DLE_DEBUG_WEAKNAMES_DUPLICATE       170
#define DW_DLE_DEBUG_WEAKNAMES_NULL            171
#define DW_DLE_DEBUG_WEAKNAMES_VERSION_ERROR   172
#define DW_DLE_DEBUG_WEAKNAMES_LENGTH_BAD      173
#define DW_DLE_WEAK_NULL                       174
#define DW_DLE_WEAK_CONTEXT_NULL               175
#define DW_DLE_LOCDESC_COUNT_WRONG             176
#define DW_DLE_MACINFO_STRING_NULL             177
#define DW_DLE_MACINFO_STRING_EMPTY            178
#define DW_DLE_MACINFO_INTERNAL_ERROR_SPACE    179
#define DW_DLE_MACINFO_MALLOC_FAIL             180
#define DW_DLE_DEBUGMACINFO_ERROR              181
#define DW_DLE_DEBUG_MACRO_LENGTH_BAD          182
#define DW_DLE_DEBUG_MACRO_MAX_BAD             183
#define DW_DLE_DEBUG_MACRO_INTERNAL_ERR        184
#define DW_DLE_DEBUG_MACRO_MALLOC_SPACE        185
#define DW_DLE_DEBUG_MACRO_INCONSISTENT        186
#define DW_DLE_DF_NO_CIE_AUGMENTATION          187
#define DW_DLE_DF_REG_NUM_TOO_HIGH             188
#define DW_DLE_DF_MAKE_INSTR_NO_INIT           189
#define DW_DLE_DF_NEW_LOC_LESS_OLD_LOC         190
#define DW_DLE_DF_POP_EMPTY_STACK              191
#define DW_DLE_DF_ALLOC_FAIL                   192
#define DW_DLE_DF_FRAME_DECODING_ERROR         193
#define DW_DLE_DEBUG_LOC_SECTION_SHORT         194
#define DW_DLE_FRAME_AUGMENTATION_UNKNOWN      195
#define DW_DLE_PUBTYPE_CONTEXT                 196 /* Unused. */
#define DW_DLE_DEBUG_PUBTYPES_LENGTH_BAD       197
#define DW_DLE_DEBUG_PUBTYPES_VERSION_ERROR    198
#define DW_DLE_DEBUG_PUBTYPES_DUPLICATE        199
#define DW_DLE_FRAME_CIE_DECODE_ERROR          200
#define DW_DLE_FRAME_REGISTER_UNREPRESENTABLE  201
#define DW_DLE_FRAME_REGISTER_COUNT_MISMATCH   202
#define DW_DLE_LINK_LOOP                       203
#define DW_DLE_STRP_OFFSET_BAD                 204
#define DW_DLE_DEBUG_RANGES_DUPLICATE          205
#define DW_DLE_DEBUG_RANGES_OFFSET_BAD         206
#define DW_DLE_DEBUG_RANGES_MISSING_END        207
#define DW_DLE_DEBUG_RANGES_OUT_OF_MEM         208
#define DW_DLE_DEBUG_SYMTAB_ERR                209
#define DW_DLE_DEBUG_STRTAB_ERR                210
#define DW_DLE_RELOC_MISMATCH_INDEX            211
#define DW_DLE_RELOC_MISMATCH_RELOC_INDEX      212
#define DW_DLE_RELOC_MISMATCH_STRTAB_INDEX     213
#define DW_DLE_RELOC_SECTION_MISMATCH          214
#define DW_DLE_RELOC_SECTION_MISSING_INDEX     215
#define DW_DLE_RELOC_SECTION_LENGTH_ODD        216
#define DW_DLE_RELOC_SECTION_PTR_NULL          217
#define DW_DLE_RELOC_SECTION_MALLOC_FAIL       218
#define DW_DLE_NO_ELF64_SUPPORT                219
#define DW_DLE_MISSING_ELF64_SUPPORT           220
#define DW_DLE_ORPHAN_FDE                      221
#define DW_DLE_DUPLICATE_INST_BLOCK            222
#define DW_DLE_BAD_REF_SIG8_FORM               223
#define DW_DLE_ATTR_EXPRLOC_FORM_BAD           224
#define DW_DLE_FORM_SEC_OFFSET_LENGTH_BAD      225
#define DW_DLE_NOT_REF_FORM                    226
#define DW_DLE_DEBUG_FRAME_LENGTH_NOT_MULTIPLE 227
#define DW_DLE_REF_SIG8_NOT_HANDLED            228
#define DW_DLE_DEBUG_FRAME_POSSIBLE_ADDRESS_BOTCH 229
#define DW_DLE_LOC_BAD_TERMINATION             230
#define DW_DLE_SYMTAB_SECTION_LENGTH_ODD       231
#define DW_DLE_RELOC_SECTION_SYMBOL_INDEX_BAD  232
#define DW_DLE_RELOC_SECTION_RELOC_TARGET_SIZE_UNKNOWN  233
#define DW_DLE_SYMTAB_SECTION_ENTRYSIZE_ZERO   234
#define DW_DLE_LINE_NUMBER_HEADER_ERROR        235
#define DW_DLE_DEBUG_TYPES_NULL                236
#define DW_DLE_DEBUG_TYPES_DUPLICATE           237
#define DW_DLE_DEBUG_TYPES_ONLY_DWARF4         238
#define DW_DLE_DEBUG_TYPEOFFSET_BAD            239
#define DW_DLE_GNU_OPCODE_ERROR                240
#define DW_DLE_DEBUGPUBTYPES_ERROR             241
#define DW_DLE_AT_FIXUP_NULL                   242
#define DW_DLE_AT_FIXUP_DUP                    243
#define DW_DLE_BAD_ABINAME                     244
#define DW_DLE_TOO_MANY_DEBUG                  245
#define DW_DLE_DEBUG_STR_OFFSETS_DUPLICATE     246
#define DW_DLE_SECTION_DUPLICATION             247
#define DW_DLE_SECTION_ERROR                   248
#define DW_DLE_DEBUG_ADDR_DUPLICATE            249
#define DW_DLE_DEBUG_CU_UNAVAILABLE_FOR_FORM   250
#define DW_DLE_DEBUG_FORM_HANDLING_INCOMPLETE  251
#define DW_DLE_NEXT_DIE_PAST_END               252
#define DW_DLE_NEXT_DIE_WRONG_FORM             253
#define DW_DLE_NEXT_DIE_NO_ABBREV_LIST         254
#define DW_DLE_NESTED_FORM_INDIRECT_ERROR      255
#define DW_DLE_CU_DIE_NO_ABBREV_LIST           256
#define DW_DLE_MISSING_NEEDED_DEBUG_ADDR_SECTION 257
#define DW_DLE_ATTR_FORM_NOT_ADDR_INDEX        258
#define DW_DLE_ATTR_FORM_NOT_STR_INDEX         259
#define DW_DLE_DUPLICATE_GDB_INDEX             260
#define DW_DLE_ERRONEOUS_GDB_INDEX_SECTION     261
#define DW_DLE_GDB_INDEX_COUNT_ERROR           262
#define DW_DLE_GDB_INDEX_COUNT_ADDR_ERROR      263
#define DW_DLE_GDB_INDEX_INDEX_ERROR           264
#define DW_DLE_GDB_INDEX_CUVEC_ERROR           265
#define DW_DLE_DUPLICATE_CU_INDEX              266
#define DW_DLE_DUPLICATE_TU_INDEX              267
#define DW_DLE_XU_TYPE_ARG_ERROR               268
#define DW_DLE_XU_IMPOSSIBLE_ERROR             269
#define DW_DLE_XU_NAME_COL_ERROR               270
#define DW_DLE_XU_HASH_ROW_ERROR               271
#define DW_DLE_XU_HASH_INDEX_ERROR             272
/* ..._FAILSAFE_ERRVAL is an aid when out of memory. */
#define DW_DLE_FAILSAFE_ERRVAL                 273
#define DW_DLE_ARANGE_ERROR                    274
#define DW_DLE_PUBNAMES_ERROR                  275
#define DW_DLE_FUNCNAMES_ERROR                 276
#define DW_DLE_TYPENAMES_ERROR                 277
#define DW_DLE_VARNAMES_ERROR                  278
#define DW_DLE_WEAKNAMES_ERROR                 279
#define DW_DLE_RELOCS_ERROR                    280
#define DW_DLE_ATTR_OUTSIDE_SECTION            281
#define DW_DLE_FISSION_INDEX_WRONG             282
#define DW_DLE_FISSION_VERSION_ERROR           283
#define DW_DLE_NEXT_DIE_LOW_ERROR              284
#define DW_DLE_CU_UT_TYPE_ERROR                285
#define DW_DLE_NO_SUCH_SIGNATURE_FOUND         286
#define DW_DLE_SIGNATURE_SECTION_NUMBER_WRONG  287
#define DW_DLE_ATTR_FORM_NOT_DATA8             288
#define DW_DLE_SIG_TYPE_WRONG_STRING           289
#define DW_DLE_MISSING_REQUIRED_TU_OFFSET_HASH 290
#define DW_DLE_MISSING_REQUIRED_CU_OFFSET_HASH 291
#define DW_DLE_DWP_MISSING_DWO_ID              292
#define DW_DLE_DWP_SIBLING_ERROR               293
#define DW_DLE_DEBUG_FISSION_INCOMPLETE        294
#define DW_DLE_FISSION_SECNUM_ERR              295
#define DW_DLE_DEBUG_MACRO_DUPLICATE           296
#define DW_DLE_DEBUG_NAMES_DUPLICATE           297
#define DW_DLE_DEBUG_LINE_STR_DUPLICATE        298
#define DW_DLE_DEBUG_SUP_DUPLICATE             299
#define DW_DLE_NO_SIGNATURE_TO_LOOKUP          300
#define DW_DLE_NO_TIED_ADDR_AVAILABLE          301
#define DW_DLE_NO_TIED_SIG_AVAILABLE           302
#define DW_DLE_STRING_NOT_TERMINATED           303
#define DW_DLE_BAD_LINE_TABLE_OPERATION        304
#define DW_DLE_LINE_CONTEXT_BOTCH              305
#define DW_DLE_LINE_CONTEXT_INDEX_WRONG        306
#define DW_DLE_NO_TIED_STRING_AVAILABLE        307
#define DW_DLE_NO_TIED_FILE_AVAILABLE          308
#define DW_DLE_CU_TYPE_MISSING                 309
#define DW_DLE_LLE_CODE_UNKNOWN                310
#define DW_DLE_LOCLIST_INTERFACE_ERROR         311
#define DW_DLE_LOCLIST_INDEX_ERROR             312
#define DW_DLE_INTERFACE_NOT_SUPPORTED         313
#define DW_DLE_ZDEBUG_REQUIRES_ZLIB            314
#define DW_DLE_ZDEBUG_INPUT_FORMAT_ODD         315
#define DW_DLE_ZLIB_BUF_ERROR                  316
#define DW_DLE_ZLIB_DATA_ERROR                 317
#define DW_DLE_MACRO_OFFSET_BAD                318
#define DW_DLE_MACRO_OPCODE_BAD                319
#define DW_DLE_MACRO_OPCODE_FORM_BAD           320
#define DW_DLE_UNKNOWN_FORM                    321
#define DW_DLE_BAD_MACRO_HEADER_POINTER        322
#define DW_DLE_BAD_MACRO_INDEX                 323
#define DW_DLE_MACRO_OP_UNHANDLED              324
#define DW_DLE_MACRO_PAST_END                  325
#define DW_DLE_LINE_STRP_OFFSET_BAD            326
#define DW_DLE_STRING_FORM_IMPROPER            327
#define DW_DLE_ELF_FLAGS_NOT_AVAILABLE         328
#define DW_DLE_LEB_IMPROPER                    329
#define DW_DLE_DEBUG_LINE_RANGE_ZERO           330
#define DW_DLE_READ_LITTLEENDIAN_ERROR         331
#define DW_DLE_READ_BIGENDIAN_ERROR            332
#define DW_DLE_RELOC_INVALID                   333
#define DW_DLE_INFO_HEADER_ERROR               334
#define DW_DLE_ARANGES_HEADER_ERROR            335
#define DW_DLE_LINE_OFFSET_WRONG_FORM          336
#define DW_DLE_FORM_BLOCK_LENGTH_ERROR         337
#define DW_DLE_ZLIB_SECTION_SHORT              338
#define DW_DLE_CIE_INSTR_PTR_ERROR             339
#define DW_DLE_FDE_INSTR_PTR_ERROR             340
#define DW_DLE_FISSION_ADDITION_ERROR          341
#define DW_DLE_HEADER_LEN_BIGGER_THAN_SECSIZE  342
#define DW_DLE_LOCEXPR_OFF_SECTION_END         343
#define DW_DLE_POINTER_SECTION_UNKNOWN         344
#define DW_DLE_ERRONEOUS_XU_INDEX_SECTION      345
#define DW_DLE_DIRECTORY_FORMAT_COUNT_VS_DIRECTORIES_MISMATCH 346
#define DW_DLE_COMPRESSED_EMPTY_SECTION        347
#define DW_DLE_SIZE_WRAPAROUND                 348
#define DW_DLE_ILLOGICAL_TSEARCH               349
#define DW_DLE_BAD_STRING_FORM                 350
#define DW_DLE_DEBUGSTR_ERROR                  351
#define DW_DLE_DEBUGSTR_UNEXPECTED_REL         352
#define DW_DLE_DISCR_ARRAY_ERROR               353
#define DW_DLE_LEB_OUT_ERROR                   354
#define DW_DLE_SIBLING_LIST_IMPROPER           355
#define DW_DLE_LOCLIST_OFFSET_BAD              356
#define DW_DLE_LINE_TABLE_BAD                  357
#define DW_DLE_DEBUG_LOClISTS_DUPLICATE        358
#define DW_DLE_DEBUG_RNGLISTS_DUPLICATE        359
#define DW_DLE_ABBREV_OFF_END                  360
#define DW_DLE_FORM_STRING_BAD_STRING          361
#define DW_DLE_AUGMENTATION_STRING_OFF_END     362
#define DW_DLE_STRING_OFF_END_PUBNAMES_LIKE    363
#define DW_DLE_LINE_STRING_BAD                 364
#define DW_DLE_DEFINE_FILE_STRING_BAD          365
#define DW_DLE_MACRO_STRING_BAD                366
#define DW_DLE_MACINFO_STRING_BAD              367
#define DW_DLE_ZLIB_UNCOMPRESS_ERROR           368
#define DW_DLE_IMPROPER_DWO_ID                 369
#define DW_DLE_GROUPNUMBER_ERROR               370
#define DW_DLE_ADDRESS_SIZE_ZERO               371
#define DW_DLE_DEBUG_NAMES_HEADER_ERROR        372
#define DW_DLE_DEBUG_NAMES_AUG_STRING_ERROR    373
#define DW_DLE_DEBUG_NAMES_PAD_NON_ZERO        374
#define DW_DLE_DEBUG_NAMES_OFF_END             375
#define DW_DLE_DEBUG_NAMES_ABBREV_OVERFLOW     376
#define DW_DLE_DEBUG_NAMES_ABBREV_CORRUPTION   377
#define DW_DLE_DEBUG_NAMES_NULL_POINTER        378
#define DW_DLE_DEBUG_NAMES_BAD_INDEX_ARG       379
#define DW_DLE_DEBUG_NAMES_ENTRYPOOL_OFFSET    380
#define DW_DLE_DEBUG_NAMES_UNHANDLED_FORM      381
#define DW_DLE_LNCT_CODE_UNKNOWN               382
#define DW_DLE_LNCT_FORM_CODE_NOT_HANDLED      383
#define DW_DLE_LINE_HEADER_LENGTH_BOTCH        384
#define DW_DLE_STRING_HASHTAB_IDENTITY_ERROR   385
#define DW_DLE_UNIT_TYPE_NOT_HANDLED           386
#define DW_DLE_GROUP_MAP_ALLOC                 387
#define DW_DLE_GROUP_MAP_DUPLICATE             388
#define DW_DLE_GROUP_COUNT_ERROR               389
#define DW_DLE_GROUP_INTERNAL_ERROR            390
#define DW_DLE_GROUP_LOAD_ERROR                391
#define DW_DLE_GROUP_LOAD_READ_ERROR           392
#define DW_DLE_AUG_DATA_LENGTH_BAD             393
#define DW_DLE_ABBREV_MISSING                  394
#define DW_DLE_NO_TAG_FOR_DIE                  395
#define DW_DLE_LOWPC_WRONG_CLASS               396
#define DW_DLE_HIGHPC_WRONG_FORM               397
#define DW_DLE_STR_OFFSETS_BASE_WRONG_FORM     398
#define DW_DLE_DATA16_OUTSIDE_SECTION          399
#define DW_DLE_LNCT_MD5_WRONG_FORM             400
#define DW_DLE_LINE_HEADER_CORRUPT             401
#define DW_DLE_STR_OFFSETS_NULLARGUMENT        402
#define DW_DLE_STR_OFFSETS_NULL_DBG            403
#define DW_DLE_STR_OFFSETS_NO_MAGIC            404
#define DW_DLE_STR_OFFSETS_ARRAY_SIZE          405
#define DW_DLE_STR_OFFSETS_VERSION_WRONG       406
#define DW_DLE_STR_OFFSETS_ARRAY_INDEX_WRONG   407
#define DW_DLE_STR_OFFSETS_EXTRA_BYTES         408
#define DW_DLE_DUP_ATTR_ON_DIE                 409
#define DW_DLE_SECTION_NAME_BIG                410
#define DW_DLE_FILE_UNAVAILABLE                411
#define DW_DLE_FILE_WRONG_TYPE                 412
#define DW_DLE_SIBLING_OFFSET_WRONG            413
#define DW_DLE_OPEN_FAIL                       414
#define DW_DLE_OFFSET_SIZE                     415
#define DW_DLE_MACH_O_SEGOFFSET_BAD            416
#define DW_DLE_FILE_OFFSET_BAD                 417
#define DW_DLE_SEEK_ERROR                      418
#define DW_DLE_READ_ERROR                      419
#define DW_DLE_ELF_CLASS_BAD                   420
#define DW_DLE_ELF_ENDIAN_BAD                  421
#define DW_DLE_ELF_VERSION_BAD                 422
#define DW_DLE_FILE_TOO_SMALL                  423
#define DW_DLE_PATH_SIZE_TOO_SMALL             424
#define DW_DLE_BAD_TYPE_SIZE                   425
#define DW_DLE_PE_SIZE_SMALL                   426
#define DW_DLE_PE_OFFSET_BAD                   427
#define DW_DLE_PE_STRING_TOO_LONG              428
#define DW_DLE_IMAGE_FILE_UNKNOWN_TYPE         429
#define DW_DLE_LINE_TABLE_LINENO_ERROR         430
#define DW_DLE_PRODUCER_CODE_NOT_AVAILABLE     431
#define DW_DLE_NO_ELF_SUPPORT                  432
#define DW_DLE_NO_STREAM_RELOC_SUPPORT         433
#define DW_DLE_RETURN_EMPTY_PUBNAMES_ERROR     434
#define DW_DLE_SECTION_SIZE_ERROR              435
#define DW_DLE_INTERNAL_NULL_POINTER           436
#define DW_DLE_SECTION_STRING_OFFSET_BAD       437
#define DW_DLE_SECTION_INDEX_BAD               438
#define DW_DLE_INTEGER_TOO_SMALL               439
#define DW_DLE_ELF_SECTION_LINK_ERROR          440
#define DW_DLE_ELF_SECTION_GROUP_ERROR         441
#define DW_DLE_ELF_SECTION_COUNT_MISMATCH      442
#define DW_DLE_ELF_STRING_SECTION_MISSING      443
#define DW_DLE_SEEK_OFF_END                    444
#define DW_DLE_READ_OFF_END                    445
#define DW_DLE_ELF_SECTION_ERROR               446
#define DW_DLE_ELF_STRING_SECTION_ERROR        447
#define DW_DLE_MIXING_SPLIT_DWARF_VERSIONS     448
#define DW_DLE_TAG_CORRUPT                     449
#define DW_DLE_FORM_CORRUPT                    450
#define DW_DLE_ATTR_CORRUPT                    451
#define DW_DLE_ABBREV_ATTR_DUPLICATION         452
#define DW_DLE_DWP_SIGNATURE_MISMATCH          453
#define DW_DLE_CU_UT_TYPE_VALUE                454
#define DW_DLE_DUPLICATE_GNU_DEBUGLINK         455
#define DW_DLE_CORRUPT_GNU_DEBUGLINK           456
#define DW_DLE_CORRUPT_NOTE_GNU_DEBUGID        457
#define DW_DLE_CORRUPT_GNU_DEBUGID_SIZE        458
#define DW_DLE_CORRUPT_GNU_DEBUGID_STRING      459
#define DW_DLE_HEX_STRING_ERROR                460
#define DW_DLE_DECIMAL_STRING_ERROR            461
#define DW_DLE_PRO_INIT_EXTRAS_UNKNOWN         462
#define DW_DLE_PRO_INIT_EXTRAS_ERR             463
#define DW_DLE_NULL_ARGS_DWARF_ADD_PATH        464
#define DW_DLE_DWARF_INIT_DBG_NULL             465
#define DW_DLE_ELF_RELOC_SECTION_ERROR         466
#define DW_DLE_USER_DECLARED_ERROR             467
#define DW_DLE_RNGLISTS_ERROR                  468
#define DW_DLE_LOCLISTS_ERROR                  469
#define DW_DLE_SECTION_SIZE_OR_OFFSET_LARGE    470
#define DW_DLE_GDBINDEX_STRING_ERROR           471
#define DW_DLE_GNU_PUBNAMES_ERROR              472
#define DW_DLE_GNU_PUBTYPES_ERROR              473
#define DW_DLE_DUPLICATE_GNU_DEBUG_PUBNAMES    474
#define DW_DLE_DUPLICATE_GNU_DEBUG_PUBTYPES    475
#define DW_DLE_DEBUG_SUP_STRING_ERROR          476
#define DW_DLE_DEBUG_SUP_ERROR                 477
#define DW_DLE_LOCATION_ERROR                  478
#define DW_DLE_DEBUGLINK_PATH_SHORT            479
#define DW_DLE_SIGNATURE_MISMATCH              480
#define DW_DLE_MACRO_VERSION_ERROR             481
#define DW_DLE_NEGATIVE_SIZE                   482
#define DW_DLE_UDATA_VALUE_NEGATIVE            483
#define DW_DLE_DEBUG_NAMES_ERROR               484
#define DW_DLE_CFA_INSTRUCTION_ERROR           485
#define DW_DLE_MACHO_CORRUPT_HEADER            486
#define DW_DLE_MACHO_CORRUPT_COMMAND           487
#define DW_DLE_MACHO_CORRUPT_SECTIONDETAILS    488
#define DW_DLE_RELOCATION_SECTION_SIZE_ERROR   489
#define DW_DLE_SYMBOL_SECTION_SIZE_ERROR       490
#define DW_DLE_PE_SECTION_SIZE_ERROR           491

/*! @note DW_DLE_LAST MUST EQUAL LAST ERROR NUMBER */
#define DW_DLE_LAST        491
#define DW_DLE_LO_USER     0x10000
/*! @} */

/*! @section initfinish Initialization And Finish Operations */

/*! @defgroup initfunctions Libdwarf Initialization Functions
    Opening and closing libdwarf on object files.
    @{
*/

/*! @brief Initialization based on path, the most common
    initialization.

    @param dw_path
    Pass in the path to the object file to open.
    @param dw_true_path_out_buffer
    Pass in NULL or the name of a string buffer
    (The buffer should be initialized with an initial NUL byte)
    The returned string will be null-terminated.
    The path actually used is copied to true_path_out.
    If true_path_buffer len is zero or true_path_out_buffer
    is zero  then the Special MacOS processing will not
    occur, nor will the GNU_debuglink processing occur.
    In case GNU debuglink data was followed or MacOS
    dSYM applies the true_path_out
    will not match path.
    So consider the value put in true_path_out the
    actual file name.
    @param dw_true_path_bufferlen
    Pass in the length in bytes of the buffer.
    @param dw_groupnumber
    The value passed in should be DW_GROUPNUMBER_ANY
    unless one wishes to other than a standard
    group.
    @param dw_errhand
    Pass in NULL unless one wishes libdwarf to call
    this error handling function (which you must write)
    instead of passing meaningfull values to the
    dw_error argument.
    @param dw_errarg
    If dw_errorhand is non-null, then this value
    (a pointer or integer that means something
    to you) is passed to the dw_errhand function
    in case that is helpful to you.
    @param dw_dbg
    On success, *dw_dbg is set to a pointer to
    a new Dwarf_Debug structure to be used in
    calls to libdwarf functions.
    @param dw_error
    In case return is DW_DLV_ERROR
    dw_error is set to point to
    the error details.
    @return DW_DLV_OK etc.

    @see dwarf_init_path_dl dwarf_init_b
    @see exampleinit
*/
DW_API int dwarf_init_path(const char * dw_path,
    char *            dw_true_path_out_buffer,
    unsigned int      dw_true_path_bufferlen,
    unsigned int      dw_groupnumber,
    Dwarf_Handler     dw_errhand,
    Dwarf_Ptr         dw_errarg,
    Dwarf_Debug*      dw_dbg,
    Dwarf_Error*      dw_error);

/*! @brief Initialization following GNU debuglink section data.

    Sets the true-path with DWARF if there is
    appropriate debuglink data available.

    @param dw_path
    Pass in the path to the object file to open.
    @param dw_true_path_out_buffer
    Pass in NULL or the name of a string buffer.
    @param dw_true_path_bufferlen
    Pass in the length in bytes of the buffer.
    @param dw_groupnumber
    The value passed in should be DW_GROUPNUMBER_ANY
    unless one wishes to other than a standard
    group.
    @param dw_errhand
    Pass in NULL, normally.
    If non-null one wishes libdwarf to call
    this error handling function (which you must write)
    instead of passing meaningful values to the
    dw_error argument.
    @param dw_errarg
    Pass in NULL, normally.
    If dw_errorhand is non-null, then this value
    (a pointer or integer that means something
    to you) is passed to the dw_errhand function
    in case that is helpful to you.
    @param dw_dbg
    On success, *dw_dbg is set to a pointer to
    a new Dwarf_Debug structure to be used in
    calls to libdwarf functions.
    @param dw_dl_path_array
    debuglink processing allows a user-specified set of file paths
    and this argument allows one to specify these.
    @param dw_dl_path_array_size
    Specify the size of the dw_dl_path_array.
    @param dw_dl_path_source
    returns  DW_PATHSOURCE_basic or other such value so the caller can
    know how the true-path was resolved.
    @param dw_error
    In case return is DW_DLV_ERROR
    dw_error is set to point to
    the error details.
    @return DW_DLV_OK etc.

    @see exampleinit_dl
    */
DW_API int dwarf_init_path_dl(const char * dw_path,
    char *            dw_true_path_out_buffer,
    unsigned int      dw_true_path_bufferlen,
    unsigned int      dw_groupnumber,
    Dwarf_Handler     dw_errhand,
    Dwarf_Ptr         dw_errarg,
    Dwarf_Debug*      dw_dbg,
    char **           dw_dl_path_array,
    unsigned int      dw_dl_path_array_size,
    unsigned char   * dw_dl_path_source,
    Dwarf_Error*      dw_error);

/*! @brief Initialization based on Unix/Linux (etc) path
    This version allows specifying any number of debuglink
    global paths to search on for debuglink targets.

    @param dw_fd
    An open Unix/Linux/etc fd on the object file.
    @param dw_groupnumber
    The value passed in should be DW_GROUPNUMBER_ANY
    unless one wishes to other than a standard
    group.
    @param dw_errhand
    Pass in NULL unless one wishes libdwarf to call
    this error handling function (which you must write)
    instead of passing meaningfull values to the
    dw_error argument.
    @param dw_errarg
    If dw_errorhand is non-null, then this value
    (a pointer or integer that means something
    to you) is passed to the dw_errhand function
    in case that is helpful to you.
    @param dw_dbg
    On success, *dw_dbg is set to a pointer to
    a new Dwarf_Debug structure to be used in
    calls to libdwarf functions.
    @param dw_error
    In case return is DW_DLV_ERROR
    dw_error is set to point to
    the error details.
    @return
    DW_DLV_OK etc.
*/
DW_API int dwarf_init_b(int dw_fd,
    unsigned int      dw_groupnumber,
    Dwarf_Handler     dw_errhand,
    Dwarf_Ptr         dw_errarg,
    Dwarf_Debug*      dw_dbg,
    Dwarf_Error*      dw_error);

/*! @brief Close the initialized dw_dbg and
    free all data libdwarf has for this dw_dbg.
    @param dw_dbg
    Close the dbg.
    @return
    May return DW_DLV_ERROR if something
    is very wrong: no further information is available..
    May return DW_DLV_NO_ENTRY
    but no further information is available.
    Normally returns DW_DLV_OK.
*/
DW_API int dwarf_finish(Dwarf_Debug dw_dbg);

/*! @brief Used to access DWARF information in memory
    or in an object format unknown to libdwarf.

    @see jitreader

    @param dw_obj
    A data structure filled out by the caller so libdwarf
    can access DWARF data not in a supported object file format.
    @param dw_errhand
    Pass in NULL normally.
    @param dw_errarg
    Pass in NULL normally.
    @param dw_groupnumber
    The value passed in should be DW_GROUPNUMBER_ANY
    unless one wishes to other than a standard
    group (quite unlikely for this interface).
    @param dw_dbg
    On success, *dw_dbg is set to a pointer to
    a new Dwarf_Debug structure to be used in
    calls to libdwarf functions.
    @param dw_error
    In case return is DW_DLV_ERROR
    dw_error is set to point to
    the error details.
    @return
    The usual value: DW_DLV_OK etc.
*/
DW_API int dwarf_object_init_b(Dwarf_Obj_Access_Interface_a* dw_obj,
    Dwarf_Handler dw_errhand,
    Dwarf_Ptr     dw_errarg,
    unsigned int  dw_groupnumber,
    Dwarf_Debug*  dw_dbg,
    Dwarf_Error*  dw_error);

/*! @brief Used to close the object_init dw_dbg.

    Close the dw_dbg opened by dwarf_object_init_b().

    @param dw_dbg
    Must be an open Dwarf_Debug opened by
    dwarf_object_init_b().
    The init call dw_obj data is not freed
    by the call to dwarf_object_finish.
    @return
    The return value DW_DLV_OK etc is pretty useless, there
    is not much you can do with it.
*/
DW_API int dwarf_object_finish(Dwarf_Debug dw_dbg);

/*! @brief Use with split dwarf.

    @param dw_basedbg
    Pass in an open dbg, on an object file
    with (normally) lots of DWARF..
    @param dw_tied_dbg
    Pass in an open dbg on an executable
    which has minimal DWARF to save space
    in the executable.
    @param dw_error
    In case return is DW_DLV_ERROR
    dw_error is set to point to
    the error details.
    @return DW_DLV_OK etc.

    @see example2
    @see example3
*/
DW_API int dwarf_set_tied_dbg(Dwarf_Debug dw_basedbg,
    Dwarf_Debug  dw_tied_dbg,
    Dwarf_Error* dw_error);

/*! @brief Use with split dwarf.

    Given a base Dwarf_Debug this returns
    the tied Dwarf_Debug.
    Unlikely anyone uses this call as
    you had the tied and base dbg when calling
    dwarf_set_tied_dbg().
*/
DW_API int dwarf_get_tied_dbg(Dwarf_Debug dw_dbg,
    Dwarf_Debug * dw_tieddbg_out,
    Dwarf_Error * dw_error);
/*! @}
*/
/*! @defgroup compilationunit CU Data-Compilation Unit (CU) Access
    Access to each CU sequentially.
    @{
*/
/*! @brief Returns information on the next CU header.

    The library keeps track of where it is in the object file
    and it knows where to find 'next'.
    @param dw_dbg
    The currend Dwarf_Debug of interest.
    @param dw_is_info
    Pass in TRUE if reading through .debug_info
    Pass in FALSE if reading through DWARF4
    .debug_types.
    @param dw_cu_header_length
    Returns the length of the just-read CU header.
    @param dw_version_stamp
    Returns the version number (2 to 5) of the CU
    header just read.
    @param dw_abbrev_offset
    Returns the .debug_abbrev offset from the
    the CU header just read.
    @param dw_address_size
    Returns the address size specified for this CU,
    usually either 4 or 8.
    @param dw_length_size
    Returns the offset size (the length
    of the size field from the header)
    specified for this CU, either 4 or 4.
    @param dw_extension_size
    If the section is standard 64bit DWARF
    then this value is 4. Else the value is zero.
    @param dw_type_signature
    If the CU is  DW_UT_skeleton DW_UT_split_compile,
    DW_UT_split_type or DW_UT_type this is the
    type signature from the CU_header
    is compied into this field.
    @param dw_typeoffset
    For DW_UT_split_type or DW_UT_type this is the
    type offset from the CU header.
    @param dw_next_cu_header_offset
    The offset in the section of the next CU
    (unless there is a compiler bug this is rarely
    of interest).
    @param dw_header_cu_type
    Returns DW_UT_compile, or other DW_UT value.
    @param dw_error
    In case return is DW_DLV_ERROR
    dw_error is set to point to
    the error details.
    @return
    Returns DW_DLV_OK on success.
    Returns DW_DLV_NO_ENTRY if all CUs have been read.
*/
DW_API int dwarf_next_cu_header_d(Dwarf_Debug dw_dbg,
    Dwarf_Bool      dw_is_info,
    Dwarf_Unsigned* dw_cu_header_length,
    Dwarf_Half*     dw_version_stamp,
    Dwarf_Off*      dw_abbrev_offset,
    Dwarf_Half*     dw_address_size,
    Dwarf_Half*     dw_length_size,
    Dwarf_Half*     dw_extension_size,
    Dwarf_Sig8*     dw_type_signature,
    Dwarf_Unsigned* dw_typeoffset,
    Dwarf_Unsigned* dw_next_cu_header_offset,
    Dwarf_Half    * dw_header_cu_type,
    Dwarf_Error*    dw_error);

/*! @brief Retrieve the first DIE or the next sibling.

    @param dw_dbg
    The Dwarf_Debug one is operating on.
    @param dw_die
    Immediately after calling dwarf_next_cu_header_d
    pass in NULL to retrieve the CU DIE.
    Or pass in a known DIE and this will retrieve
    the next sibling in the chain.
    @param dw_is_info
    Pass TRUE or FALSE to match the applicable
    dwarf_next_cu_header_d call.
    @param dw_return_siblingdie
    The DIE returned through the pointer.
    @param dw_error
    The usual error information, if any.
    @return
    Returns DW_DLV_OK etc.

    @see dwarf_get_die_infotypes
*/
DW_API int dwarf_siblingof_b(Dwarf_Debug dw_dbg,
    Dwarf_Die        dw_die,
    Dwarf_Bool       dw_is_info,
    Dwarf_Die*       dw_return_siblingdie,
    Dwarf_Error*     dw_error);

/*! @brief Some CU-relative facts.

    Any Dwarf_Die will work.
    The values returned through the pointers
    are about the CU for a DIE
    @param dw_die
    Some open Dwarf_Die.
    @param dw_version
    Returns the DWARF version: 2,4,5, or 5
    @param dw_is_info
    Returns non-zero if the CU is .debug_info.
    Returns zero if the CU is .debug_types (DWARF4).
    @param dw_is_dwo
    Returns ton-zero if the CU is a dwo/dwp object and
    zero if it is a standard object.
    @param dw_offset_size
    Returns offset size, 4 and 8 are possible.
    @param dw_address_size
    Almost always returns 4 or 8. Could be 2
    in unusual circumstances.
    @param dw_extension_size
    The sum of dw_offset_size and dw_extension_size
    are the count of the initial bytes of the CU.
    Standard lengths are 4 and 12.
    For 1990's SGI objects the length could be 8.
    @param dw_signature
    Returns a pointer to an 8 byte signature.
    @param dw_offset_of_length
    Returns the section offset of the initial
    byte of the CU.
    @param dw_total_byte_length
    Returns the total length of the CU including
    the length field and the content of the CU.
    @param dw_error
    The usual Dwarf_Error*.
    @return
    Returns DW_DLV_OK etc.
*/

DW_API int dwarf_cu_header_basics(Dwarf_Die dw_die,
    Dwarf_Half     * dw_version,
    Dwarf_Bool     * dw_is_info,
    Dwarf_Bool     * dw_is_dwo,
    Dwarf_Half     * dw_offset_size,
    Dwarf_Half     * dw_address_size,
    Dwarf_Half     * dw_extension_size,
    Dwarf_Sig8    ** dw_signature,
    Dwarf_Off      * dw_offset_of_length,
    Dwarf_Unsigned * dw_total_byte_length,
    Dwarf_Error    * dw_error);

/*! @brief Get the child DIE, if any.
    The child may be the first of a list of
    sibling DIEs.

    @param dw_die
    We will return the first child of this DIE.
    @param dw_return_childdie
    Returns the first child through the pointer.
    For subsequent dies siblings of the first, use
    dwarf_siblingof_b().
    @param dw_error
    The usual Dwarf_Error*.
    @return
    Returns DW_DLV_OK etc. Returns DW_DLV_NO_ENTRY
    if dw_die has no children.
*/

DW_API int dwarf_child(Dwarf_Die dw_die,
    Dwarf_Die*    dw_return_childdie,
    Dwarf_Error*  dw_error);

/*! @brief  Deallocate (free) a DIE.
    @param dw_die
    Frees (deallocs) memory associated with this Dwarf_Die.
*/
DW_API void dwarf_dealloc_die( Dwarf_Die dw_die);

/*! @brief Given a has signature, retrieve the applicable CU die

    @param dw_dbg
    @param dw_hash_sig
    A pointer to an 8 byte signature to be looked up.
    in .debug_names.
    @param dw_sig_type
    Valid type requests are "cu" and "tu"
    @param dw_returned_CU_die
    Returnes the found CU DIE if one is found.
    @param dw_error
    The usual Dwarf_Error*.
    @return
    DW_DLV_OK means dw_returned_CU_die was set.
    DW_DLV_NO_ENTRY means  the signature could
    not be found.
*/
DW_API int dwarf_die_from_hash_signature(Dwarf_Debug dw_dbg,
    Dwarf_Sig8 *  dw_hash_sig,
    const char *  dw_sig_type,
    Dwarf_Die*    dw_returned_CU_die,
    Dwarf_Error*  dw_error);

/*! @brief Finding die given global (not CU-relative) offset.

    This works whether or not the target section
    has had  dwarf_next_cu_header_d() applied,
    the CU the offset exists in has
    been seen at all, or the target offset is one
    libdwarf has seen before.

    @param dw_dbg
    The applicable Dwarf_Debug
    @param dw_offset
    The global offset of the DIE in the appropriate
    section.
    @param dw_is_info
    Pass TRUE if the target is .debug_info, else
    pass FALSE if the target is .debug_types.
    @param dw_return_die
    On  success this returns a DIE pointer to
    the found DIE.
    @param dw_error
    The usual Dwarf_Error*.
    @return
    DW_DLV_OK means dw_returned_die was found
    DW_DLV_NO_ENTRY is only possible if the offset
    is to a null DIE, and that is very unusual.
    Otherwise expect DW_DLV_ERROR.

*/
DW_API int dwarf_offdie_b(Dwarf_Debug dw_dbg,
    Dwarf_Off        dw_offset,
    Dwarf_Bool       dw_is_info,
    Dwarf_Die*       dw_return_die,
    Dwarf_Error*     dw_error);

/*! @brief Retrieves a DIE from a DW_UT_split_type
    or DW_UT_type CU.

returns DIE and
    is_info flag if it finds the referenced
    DW_UT_split_type or DW_UT_type CU.

    @param dw_dbg
    The applicable Dwarf_Debug
    @param dw_ref
    A pointer to a Dwarf_Sig8 struct whose
    content defines what is being searched for.
    @param dw_die_out
    If found, this returns the found DIE itself.
    @param dw_is_info
    If found, this returns section (.debug_is_info
    or .debug_is_types).
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_find_die_given_sig8(Dwarf_Debug dw_dbg,
    Dwarf_Sig8 * dw_ref,
    Dwarf_Die  * dw_die_out,
    Dwarf_Bool * dw_is_info,
    Dwarf_Error* dw_error);

/*! @brief Returns the is_info flag.

    So client software knows if a DIE is in debug_info or debug_types.
    @param dw_die
    The DIE being queried.
    @return
    If non-zero the flag means the DIE is in .debug_info.
    Otherwise it means the DIE is in .debug_types.
*/
DW_API Dwarf_Bool dwarf_get_die_infotypes_flag(Dwarf_Die dw_die);
/*! @} */

/*! @defgroup dieentry CU Data-Debugging Information Entry Access
    This is the main interface to attributes of a DIE.
    @{
*/

/*! @brief
    So we can associate a DIE's abbreviations with the contents
    the abbreviations section.
    Useful for detailed printing and analysis of
    abbreviations

    @param dw_die
    The DIE of interest
    @param dw_abbrev_offset
    On success is set to the global offset
    in the .debug_abbrev section of the abbreviations
    for the DIE.
    @param dw_abbrev_count
    On success is set to the count of abbreviations
    in the .debug_abbrev section of the abbreviations
    for the DIE.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_die_abbrev_global_offset(Dwarf_Die dw_die,
    Dwarf_Off       * dw_abbrev_offset,
    Dwarf_Unsigned  * dw_abbrev_count,
    Dwarf_Error*      dw_error);

/*! @brief  Get TAG value of DIE.
    @param dw_die
    The DIE of interest
    @param dw_return_tag
    On success, set to the DW_TAG value of the DIE.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_tag(Dwarf_Die dw_die,
    Dwarf_Half*      dw_return_tag,
    Dwarf_Error*     dw_error);

/*! @brief Returns the global section offset of the DIE.

    @param dw_die
    The DIE of interest
    @param dw_return_offset
    On success the offset refers to the section of the DIE
    itself, which may be .debug_offset or .debug_types.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_dieoffset(Dwarf_Die dw_die,
    Dwarf_Off*       dw_return_offset,
    Dwarf_Error*     dw_error);

/*! @brief Extract address given address index. DWARF5

    @param dw_die
    The DIE of interest
    @param dw_index
    An index into .debug_addr.  This will look first for
    .debug_addr in the dbg object DIE and if not there
    will look in the tied object if that is available.
    @param dw_return_addr
    On success the address is returned through the pointer.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_debug_addr_index_to_addr(Dwarf_Die dw_die,
    Dwarf_Unsigned  dw_index,
    Dwarf_Addr    * dw_return_addr,
    Dwarf_Error   * dw_error);

/*! @brief Informs if a DW_FORM is an indexed form

    Reading a CU DIE with DW_AT_low_pc an indexed value can
    be problematic as several different FORMs are indexed.
    Some in DWARF5 others being extensions to DWARF4
    and DWARF5. Indexed forms interact with DW_AT_addr_base
    in a DIE making this a very relevant distinction.
*/
DW_API Dwarf_Bool dwarf_addr_form_is_indexed(int dw_form);

/*! @brief Returns the CU DIE offset given any DIE

    Returns
    the global debug_info section offset of the CU die
    in the CU containing the given_die
    (the passed in DIE can be any DIE).

    @see dwarf_get_cu_die_offset_given_cu_header_offset_b

    @param dw_die
    The die being queried.
    @param dw_return_offset
    Returns the section offset of the CU DIE for dw_die.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_CU_dieoffset_given_die(Dwarf_Die dw_die,
    Dwarf_Off*       dw_return_offset,
    Dwarf_Error*     dw_error);

/*! @brief Returns the CU DIE section offset given CU header offset

    Returns the CU die global offset if one knows the
    CU header global offset.
    @see dwarf_CU_dieoffset_given_die

    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_in_cu_header_offset
    The CU header offset.
    @param dw_is_info
    If TRUE the CU header offset is in .debug_info.
    Otherwise the CU header offset is in .debug_types.
    @param dw_out_cu_die_offset
    The CU DIE offset returned through this pointer.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_cu_die_offset_given_cu_header_offset_b(
    Dwarf_Debug  dw_dbg,
    Dwarf_Off    dw_in_cu_header_offset,
    Dwarf_Bool   dw_is_info,
    Dwarf_Off *  dw_out_cu_die_offset,
    Dwarf_Error *dw_error);

/*! @brief returns the CU relative offset of the DIE.

    @see dwarf_CU_dieoffset_given_die

    @param dw_die
    The DIE being queried.
    @param dw_return_offset
    Returns the CU relative offset of this DIE.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_die_CU_offset(Dwarf_Die dw_die,
    Dwarf_Off*       dw_return_offset,
    Dwarf_Error*     dw_error);

/*! @brief Returns the offset length of the entire CU of a DIE.
    @param dw_die
    The DIE being queried.
    @param dw_return_CU_header_offset
    On success returns the section offset of the CU
    this DIE is in.
    @param dw_return_CU_length_bytes
    On success returns the CU length of the CU
    this DIE is in, including the CU length, header,
    and all DIEs.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_die_CU_offset_range(Dwarf_Die dw_die,
    Dwarf_Off*   dw_return_CU_header_offset,
    Dwarf_Off*   dw_return_CU_length_bytes,
    Dwarf_Error* dw_error);

/*! @brief Given DIE and attribute number return a Dwarf_attribute

    Returns DW_DLV_NO_ENTRY if the DIE has no attribute dw_attrnum.

    @param dw_die
    The DIE of interest.
    @param dw_attrnum
    An attribute number, for example DW_AT_name.
    @param dw_returned_attr
    On success a Dwarf_Attribute pointer is returned
    and it should eventually be deallocated.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.

*/
DW_API int dwarf_attr(Dwarf_Die dw_die,
    Dwarf_Half        dw_attrnum,
    Dwarf_Attribute * dw_returned_attr,
    Dwarf_Error*      dw_error);

/*! @brief Given DIE and attribute number return a string

    Returns DW_DLV_NO_ENTRY if the DIE has no attribute dw_attrnum.

    @param dw_die
    The DIE of interest.
    @param dw_attrnum
    An attribute number, for example DW_AT_name.
    @param dw_ret_name
    On success a pointer to the string
    is returned.
    Do not free the string.
    Many attributes allow various forms that directly or
    indirectly contain strings and this
    follows all of them to their string.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.

*/
DW_API int dwarf_die_text(Dwarf_Die dw_die,
    Dwarf_Half    dw_attrnum,
    char       ** dw_ret_name,
    Dwarf_Error * dw_error);

/*! @brief Return the string from a DW_AT_name attribute

    Returns DW_DLV_NO_ENTRY if the DIE has no attribute
    DW_AT_name

    @param dw_die
    The DIE of interest.
    @param dw_diename
    On success a pointer to the string
    is returned.
    Do not free the string.
    Various forms directly or
    indirectly contain strings and this
    follows all of them to their string.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_diename(Dwarf_Die dw_die,
    char   **        dw_diename,
    Dwarf_Error*     dw_error);

/*! @brief Return the DIE abbrev code

    The Abbrev code for a DIE is an integer assigned
    by the compiler within a particular CU.
    For .debug_names abbreviations the
    situation is different.

    Returns the  abbrev code of the die. Cannot fail.

    @param dw_die
    The DIE of interest.
    @return
    The abbrev code. of the DIE.
*/
DW_API int dwarf_die_abbrev_code(Dwarf_Die dw_die);

/*! @brief Returns TRUE if the DIE has children

    @param dw_die
    A DIE.
    @param dw_ab_has_child
    Sets TRUE though the pointer if the DIE
    has children.
    Otherwise sets FALSE.
    @return
    Returns TRUE if the DIE has a child DIE.
    Else returns FALSE.

*/
DW_API int dwarf_die_abbrev_children_flag(Dwarf_Die dw_die,
    Dwarf_Half * dw_ab_has_child);

/*!  @brief Validate a sibling DIE.

    This is used by dwarfdump (when
    dwarfdump is checking for valid DWARF but
    it depends on the caller to have done a lot
    of precise setup. Ignore it. It has to change.
    REPLACEME
*/
DW_API int dwarf_validate_die_sibling(Dwarf_Die dw_sibling,
    Dwarf_Off* dw_offset);

/* convenience functions, alternative to using dwarf_attrlist */

/*! @brief Tells whether a DIE has a particular attribute.

    @param dw_die
    The DIE of interest.
    @param dw_attrnum
    The attribute number we are asking about,
    DW_AT_name for example.
    @param dw_returned_bool
    On success is set TRUE if dw_die has
    dw_attrnum.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.

*/
DW_API int dwarf_hasattr(Dwarf_Die dw_die,
    Dwarf_Half   dw_attrnum,
    Dwarf_Bool * dw_returned_bool,
    Dwarf_Error* dw_error);

/*! @brief Returns an array of DIE children offsets

    Given a DIE offset and dw_is_info,
    returns an array of DIE offsets of the childred
    of DIE.
    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_offset
    A DIE offset.
    @param dw_is_info
    If TRUE says to use the offset in .debug_info.
    Else .debug_types.
    @param dw_offbuf
    A pointer to an array of offsets is returned
    through the pointer.
    @param dw_offcount
    The number of elements in offbuf.
    IF the DIE has no children it could
    be zero, in which case offbuf
    and dw_offcount are not touched.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
    DW_DLV_NO_ENTRY means there are no children of the DIE,
    hence no list of child offsets.

    @see exampleoffsetlist
*/
DW_API int dwarf_offset_list(Dwarf_Debug dw_dbg,
    Dwarf_Off         dw_offset,
    Dwarf_Bool        dw_is_info,
    Dwarf_Off      ** dw_offbuf,
    Dwarf_Unsigned *  dw_offcount,
    Dwarf_Error    *  dw_error);

/*! @brief Get the address size applying to a DIE

    @param dw_die
    The DIE of interest.
    @param dw_addr_size
    On success, returns the address size that applies
    to dw_die. Normally 4 or 8.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_die_address_size(Dwarf_Die dw_die,
    Dwarf_Half  * dw_addr_size,
    Dwarf_Error * dw_error);

/* Get both offsets (local and global) */
/*! @brief Return section and CU-local offsets of a DIE
    @param dw_die
    The DIE of interest.
    @param dw_global_offset
    On success returns the offset of the DIE in
    its section.
    @param dw_local_offset
    On success returns the offset of the DIE within
    its CU.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_die_offsets(Dwarf_Die dw_die,
    Dwarf_Off*    dw_global_offset,
    Dwarf_Off*    dw_local_offset,
    Dwarf_Error*  dw_error);

/*! @brief Get the version and offset size

    The values returned apply to the CU this DIE
    belongs to.
    This is useful as preparation for calling
    dwarf_get_form_class

    @param dw_die
    The DIE of interest.
    @param dw_version
    Returns the version of the CU this DIE is contained in.
    Standard version numbers are 2 through 5.
    @param dw_offset_size
    Returns the offset_size (4 or 8) of the CU
    this DIE is contained in.
*/
DW_API int dwarf_get_version_of_die(Dwarf_Die dw_die,
    Dwarf_Half * dw_version,
    Dwarf_Half * dw_offset_size);

/*! @brief Returns the DW_AT_low_pc  value

    @param dw_die
    The DIE of interest.
    @param dw_returned_addr
    On success returns, through the pointer,
    the address DW_AT_low_pc defines.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.

*/
DW_API int dwarf_lowpc(Dwarf_Die dw_die,
    Dwarf_Addr * dw_returned_addr,
    Dwarf_Error* dw_error);

/*  When the highpc attribute is of class  'constant'
    it is not an address, it is an offset from the
    base address (such as lowpc) of the function.
    This is therefore a required interface for DWARF4
    style DW_AT_highpc.  */

/*! @brief Returns the DW_AT_hipc  address value

    Calculating the high pc involves several elements
    which we don't describe here. See the DWARF5 standard.
    This is accessing the DW_AT_high_pc attribute.
    @param dw_die
    The DIE of interest.
    @param dw_return_addr
    On success returns the  high-pc address for this DIE.
    @param dw_return_form
    On success returns the actual FORM for this attribute.
    @param dw_return_class
    On success returns the FORM CLASS for this attribute.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_highpc_b(Dwarf_Die dw_die,
    Dwarf_Addr  *           dw_return_addr,
    Dwarf_Half  *           dw_return_form,
    enum Dwarf_Form_Class * dw_return_class,
    Dwarf_Error *           dw_error);

/*! @brief Returns the offset from the DW_AT_type attribute

    The offset returned is is a global offset of a type DIE.
    If this CU is DWARF4 the offset would be in
    .debug_types, otherwise it is in .debug_info.

    @param dw_die
    The DIE of interest.
    @param dw_return_offset
    If successful, returns the offset through the pointer.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_dietype_offset(Dwarf_Die dw_die,
    Dwarf_Off   * dw_return_offset,
    Dwarf_Error * dw_error);

/*! @brief Returns the value of the attribute  DW_AT_byte_size

    @param dw_die
    The DIE of interest.
    @param dw_returned_size
    If successful, returns the size through the pointer.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_bytesize(Dwarf_Die dw_die,
    Dwarf_Unsigned * dw_returned_size,
    Dwarf_Error*     dw_error);

/*! @brief Returns the value of the attribute  DW_AT_bitsize

    @param dw_die
    The DIE of interest.
    @param dw_returned_size
    If successful, returns the size through the pointer.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_bitsize(Dwarf_Die dw_die,
    Dwarf_Unsigned * dw_returned_size,
    Dwarf_Error*     dw_error);

/*! @brief Returns the bit offset attribute of a DIE

    If the attribute is DW_AT_data_bit_offset
    (DWARF4, DWARF5) the returned bit offset
    has one meaning.
    If the attribute is DW_AT_bit_offset
    (DWARF2, DWARF3) the meaning is quite different.

    @param dw_die
    The DIE of interest.
    @param dw_attrnum
    If successful, returns the number of the attribute
    (DW_AT_data_bit_offset or DW_AT_bit_offset)
    @param dw_returned_offset
    If successful, returns the bit offset value.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_bitoffset(Dwarf_Die dw_die,
    Dwarf_Half     * dw_attrnum,
    Dwarf_Unsigned * dw_returned_offset,
    Dwarf_Error*     dw_error);
/*! @brief Returns the value of the DW_AT_language attribute.

    The DIE should be a CU DIE.
    @param dw_die
    The DIE of interest.
    @param dw_returned_lang
    On success returns the language code (normally
    only found on a CU DIE). For example DW_LANG_C
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_srclang(Dwarf_Die dw_die,
    Dwarf_Unsigned * dw_returned_lang,
    Dwarf_Error    * dw_error);

/*! @brief Returns the value of the DW_AT_ordering attribute.

    @param dw_die
    The DIE of interest.
    @param dw_returned_order
    On success returns the ordering value.
    For example DW_ORD_row_major
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_arrayorder(Dwarf_Die dw_die,
    Dwarf_Unsigned * dw_returned_order,
    Dwarf_Error*     dw_error);

/*! @} */
/*! @defgroup attrform CU Data-Attribute and Attribute-Form Details
    Access to the details of DIEs
    @{
*/
/*! @brief Gets the full list of attributes

    @param dw_die
    The DIE from which to pull attributes.
    @param dw_attrbuf
    The pointer is set to point to an array
    of Dwarf_Attribute (pointers to attribute data).
    This array must eventually be deallocated.
    @param dw_attrcount
    The number of entries in the array of pointers.
    There is no null-pointer to terminate the list,
    use this count.
    @param dw_error
    A place to return error details.
    @return
    If it returns DW_DLV_ERROR and dw_error is non-null
    it creates an Dwarf_Error and places it in this argument.
    Usually returns DW_DLV_OK.
    @see example1
*/
DW_API int dwarf_attrlist(Dwarf_Die dw_die,
    Dwarf_Attribute** dw_attrbuf,
    Dwarf_Signed * dw_attrcount,
    Dwarf_Error*   dw_error);

/*! @brief Sets TRUE of a Dwarf_Attribute has the indicated FORM
    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_form
    The DW_FORM you are asking about, DW_FORM_strp for example.
    @param dw_returned_bool
    On success, sets the value to TRUE or FALSE.
    @param dw_error
    A place to return error details.
    @return
    Returns DW_DLV_OK and sets dw_returned_bool.
    If attribute is passed in NULL or
    the attribute is badly broken
    the call returns DW_DLV_ERROR.
    Never returns DW_DLV_NO_ENTRY;
*/
DW_API int dwarf_hasform(Dwarf_Attribute dw_attr,
    Dwarf_Half   dw_form,
    Dwarf_Bool * dw_returned_bool,
    Dwarf_Error* dw_error);

/*! @brief Returns the form of the Dwarf_Attribute

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_returned_final_form
    The form of the item is returned
    through the pointer. If the base form is DW_FORM_indirect
    the function resolves the final form and
    returns that final form.
    @param dw_error
    A place to return error details.
    @return
    Returns DW_DLV_OK and sets dw_returned_final_form
    If attribute is passed in NULL or
    the attribute is badly broken
    the call returns DW_DLV_ERROR.
    Never returns DW_DLV_NO_ENTRY;
*/
DW_API int dwarf_whatform(Dwarf_Attribute dw_attr,
    Dwarf_Half * dw_returned_final_form,
    Dwarf_Error* dw_error);

/*! @brief Returns the initial form of the Dwarf_Attribute
    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_returned_initial_form
    The form of the item is returned
    through the pointer. If the base form is DW_FORM_indirect
    the value set is DW_FORM_indirect.
    @param dw_error
    A place to return error details.
    @return
    Returns DW_DLV_OK and sets dw_returned_initial_form.
    If attribute is passed in NULL or
    the attribute is badly broken
    the call returns DW_DLV_ERROR.
    Never returns DW_DLV_NO_ENTRY;
*/
DW_API int dwarf_whatform_direct(Dwarf_Attribute dw_attr,
    Dwarf_Half * dw_returned_initial_form,
    Dwarf_Error* dw_error);

/*! @brief Returns the attribute number of the Dwarf_Attribute
    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_returned_attrnum
    The attribute number of the attribute is returned
    through the pointer. For example, DW_AT_name
    @param dw_error
    A place to return error details.
    @return
    Returns DW_DLV_OK and sets dw_returned_attrnum
    If attribute is passed in NULL or
    the attribute is badly broken
    the call returns DW_DLV_ERROR.
    Never returns DW_DLV_NO_ENTRY;
*/
DW_API int dwarf_whatattr(Dwarf_Attribute dw_attr,
    Dwarf_Half * dw_returned_attrnum,
    Dwarf_Error* dw_error);

/*! @brief Retrieve the CU-relative offset of a reference

    The DW_FORM of the attribute must be one of a small set
    of local reference forms: DW_FORM_ref<n> or
    DW_FORM_udata.

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_return_offset
    Returns the CU-relative offset through the pointer.
    @param dw_is_info
    Returns a flag through the pointer. TRUE if the
    offset is in .debug_info, FALSE if it is in .debug_types
    @param dw_error
    A place to return error details.
    @return
    Returns DW_DLV_OK and sets dw_returned_attrnum
    If attribute is passed in NULL or
    the attribute is badly broken or
    the FORM of this attribute is not one of the small
    set of local references
    the call returns DW_DLV_ERROR.
    Never returns DW_DLV_NO_ENTRY;
*/
DW_API int dwarf_formref(Dwarf_Attribute dw_attr,
    Dwarf_Off*   dw_return_offset,
    Dwarf_Bool * dw_is_info,
    Dwarf_Error* dw_error);

/*! @brief Return the section-relative offset of a Dwarf_Attribute

    The target section of the returned offset can be in
    various sections depending on the FORM.
    Only a DW_FORM_ref_sig8 can change the returned offset of
    a .debug_info die via a lookup into .debug_types
    by changing dw_offset_is_info to FALSE (DWARF4).

    The caller must determine the target section from
    the FORM.

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_return_offset
    Returns the CU-relative offset through the pointer.
    @param dw_offset_is_info
    For references to DIEs this informs whether the target
    DIE (the target the offset refers to) is in .debug_info
    or .debug_types.  For non-DIE targets this field
    is not meaningful. Refer to the attribute FORM to determin
    the target section of the offset.
    @param dw_error
    A place to return error details.
    @return
    Returns DW_DLV_OK and sets dw_return_offset
    and dw_offset_is_info.
    If attribute is passed in NULL or
    the attribute is badly broken or
    the FORM of this attribute is not one of the
    many reference types the call returns DW_DLV_ERROR.
    Never returns DW_DLV_NO_ENTRY;
*/
DW_API int dwarf_global_formref_b(Dwarf_Attribute dw_attr,
    Dwarf_Off  * dw_return_offset,
    Dwarf_Bool * dw_offset_is_info,
    Dwarf_Error* dw_error);

/*! @brief Same as dwarf_global_formref_b except...

    @see  dwarf_global_formref_b

    This is the same, except there is no
    dw_offset_is_info pointer so in the case of
    DWARF4 and DW_FORM_ref_sig8 it is not
    possible to determine which section the offset
    applies to!
*/
DW_API int dwarf_global_formref(Dwarf_Attribute dw_attr,
    Dwarf_Off*       dw_return_offset,
    Dwarf_Error*     dw_error);

/*! @brief Return an 8 byte reference form for DW_FORM_ref_sig8

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_returned_sig_bytes
    On success returns DW_DLV_OK and copies the 8 bytes
    into dw_returned_sig_bytes.
    @param dw_error
    A place to return error details.
    @return
    On success returns DW_DLV_OK and copies the 8 bytes
    into dw_returned_sig_bytes.
    If attribute is passed in NULL or
    the attribute is badly broken
    the call returns DW_DLV_ERROR.
    If the dw_attr has a form other than DW_FORM_ref_sig8
    the function returns DW_DLV_NO_ENTRY
*/
DW_API int dwarf_formsig8(Dwarf_Attribute dw_attr,
    Dwarf_Sig8 * dw_returned_sig_bytes,
    Dwarf_Error* dw_error);

/*! @brief Return an 8 byte reference form for DW_FORM_data8

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_returned_sig_bytes
    On success Returns DW_DLV_OK and copies the 8 bytes
    into dw_returned_sig_bytes.
    @param dw_error
    A place to return error details.
    @return
    On success returns DW_DLV_OK and copies the 8 bytes
    into dw_returned_sig_bytes.
    If attribute is passed in NULL or
    the attribute is badly broken
    the call returns DW_DLV_ERROR.
    If the dw_attr has a form other than DW_FORM_data8
    the function returns DW_DLV_NO_ENTRY
*/
DW_API int dwarf_formsig8_const(Dwarf_Attribute dw_attr,
    Dwarf_Sig8 * dw_returned_sig_bytes,
    Dwarf_Error* dw_error);

/*! @brief Return the address when the attribute has form address.

    There are several address forms, some of them indexed.

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_returned_addr
    On success this set through the pointer to
    the address in the attribute.
    @param dw_error
    A place to return error details.
    @return
    On success returns DW_DLV_OK sets dw_returned_addr .
    If attribute is passed in NULL or
    the attribute is badly broken
    or the address cannot be retrieved
    the call returns DW_DLV_ERROR.
    Never returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_formaddr(Dwarf_Attribute dw_attr,
    Dwarf_Addr * dw_returned_addr,
    Dwarf_Error* dw_error);

/*! @brief Get the addr index of a Dwarf_Attribute

    So a consumer can get the index when
    the object with the actual .debug_addr section is
    elsewhere (Debug Fission). Or if the caller just wants the index.
    Only call it when you know it should does have an index
    address FORM such as DW_FORM_addrx1 or one of the GNU
    address index forms.

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_return_index
    If successful it returns the index
    through the pointer.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds. Never returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_get_debug_addr_index(Dwarf_Attribute dw_attr,
    Dwarf_Unsigned * dw_return_index,
    Dwarf_Error *    dw_error);

/*! @brief Return the flag value of a flag form.

    It is an error if the FORM is not a flag form.

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_returned_bool
    Returns either TRUE or FALSE through the pointer.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds. Never returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_formflag(Dwarf_Attribute dw_attr,
    Dwarf_Bool *     dw_returned_bool,
    Dwarf_Error*     dw_error);

/*! @brief Return an unsigned value

    The form can be an unsigned or signed integral type
    but if it is a signed type the value must be non-negative.
    It is an error otherwise.

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_returned_val
    On success returns the unsigned value through the pointer.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds. Never returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_formudata(Dwarf_Attribute dw_attr,
    Dwarf_Unsigned * dw_returned_val,
    Dwarf_Error*     dw_error);

/*! @brief Return a signed value

    The form  must be a signed integral type.
    It is an error otherwise.

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_returned_val
    On success returns the signed value through the pointer.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds. Never returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_formsdata(Dwarf_Attribute dw_attr,
    Dwarf_Signed  * dw_returned_val,
    Dwarf_Error*    dw_error);

/*! @brief Return a 16 byte Dwarf_Form_Data16 value.

    We just store the bytes in a struct, we have no
    16 byte integer type.
    It is an error if the FORM is not DW_FORM_data16
    @see Dwarf_Form_Data16

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_returned_val
    Copies the 16 byte value into the pointed
    to area.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds. Never returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_formdata16(Dwarf_Attribute dw_attr,
    Dwarf_Form_Data16 * dw_returned_val,
    Dwarf_Error*        dw_error);

/*! @brief Returns an allocated filled-in Form_Block

    It is an error if the DW_FORM in the attribute is not a
    block form. DW_FORM_block2 is an example of a block form.

    @see Dwarf_Block
    @see examplediscrlist

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_returned_block
    Allocates a Dwarf_Block and returns
    a pointer to the filled-in block.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds. Never returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_formblock(Dwarf_Attribute dw_attr,
    Dwarf_Block ** dw_returned_block,
    Dwarf_Error*   dw_error);

/*! @brief Returns a pointer to a string.

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_returned_string
    Puts a pointer to a string in the DWARF information
    if the FORM of the attribute is some sort of string FORM.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_formstring(Dwarf_Attribute dw_attr,
    char   **        dw_returned_string,
    Dwarf_Error*     dw_error);

/*! @brief Returns a string index
    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_return_index
    If the form is a string index form (for example
    DW_FORM_strx) the string index value is
    returned via the pointer.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
    If the attribute form is not one of the string index
    forms it returns DW_DLV_ERROR and sets dw_error to point
    to the error details.
*/
DW_API int dwarf_get_debug_str_index(Dwarf_Attribute dw_attr,
    Dwarf_Unsigned * dw_return_index,
    Dwarf_Error *    dw_error);

/*! @brief Returns a pointer-to and length-of a block of data.

    @param dw_attr
    The Dwarf_Attribute of interest.
    @param dw_return_exprlen
    Returns the length in bytes of the block
    if it succeeds.
    @param dw_block_ptr
    Returns a pointer to the first byte of the block
    of data if it succeeds.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
    If the attribute form is not DW_FORM_exprloc
    it returns DW_DLV_ERROR and sets dw_error to point
    to the error details.
*/
DW_API int dwarf_formexprloc(Dwarf_Attribute dw_attr,
    Dwarf_Unsigned * dw_return_exprlen,
    Dwarf_Ptr      * dw_block_ptr,
    Dwarf_Error    * dw_error);

/*! @brief Returns the FORM_CLASS applicable.
    Four pieces of information are necessary
    to get the correct FORM_CLASS.

    @param dw_version
    The CU's DWARF version. Standard numbers are 2,3,4, or 5.
    @param dw_attrnum
    For example DW_AT_name
    @param dw_offset_size
    The offset size applicable to the compilation unit
    relevant to the attribute and form.
    @param dw_form
    The FORM number, for example DW_FORM_data4
    @return
    Returns a form class, for example DW_FORM_CLASS_CONSTANT.
    The FORM_CLASS names are mentioned (for example
    as 'address' in Table 2.3 of DWARF5) but
    are not assigned formal names & numbers in the standard.
*/
DW_API enum Dwarf_Form_Class dwarf_get_form_class(
    Dwarf_Half dw_version,
    Dwarf_Half dw_attrnum,
    Dwarf_Half dw_offset_size,
    Dwarf_Half dw_form);

/*! @brief Returns the offset of an attribute in its section

    @param dw_die
    The DIE of interest.
    @param dw_attr
    A Dwarf_Attribute of interest in this DIE
    @param dw_return_offset
    The offset is in .debug_info if the DIE is there.
    The offset is in .debug_types if the DIE is there.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
    DW_DLV_NO_ENTRY is impossible.
*/
DW_API int dwarf_attr_offset(Dwarf_Die dw_die,
    Dwarf_Attribute dw_attr,
    Dwarf_Off     * dw_return_offset,
    Dwarf_Error   * dw_error);

/*! @brief Uncompress a block of sleb numbers
    It's not much of a compression so not much
    of an uncompression.
    Developed by Sun Microsystems and it is unclear if it
    was ever used.
    @see dwarf_dealloc_uncompressed_block
*/
DW_API int dwarf_uncompress_integer_block_a(Dwarf_Debug dw_dbg,
    Dwarf_Unsigned   dw_input_length_in_bytes,
    void           * dw_input_block,
    Dwarf_Unsigned * dw_value_count,
    Dwarf_Signed  ** dw_value_array,
    Dwarf_Error    * dw_error);

/*! @brief dealloc what dwarf_uncompress_integer_block_a allocated
    @param dw_dbg
    The Dwarf_Debug of interest
    @param dw_value_array
    The array was called an array of Dwarf_Signed.
    We dealloc all of it without needing dw_value_count.
*/
DW_API void dwarf_dealloc_uncompressed_block(Dwarf_Debug dw_dbg,
    void *dw_value_array);

/*! @brief Convert local offset to global offset

    Uses the DW_FORM of the attribute to determine
    if the dw_offset is local, and if so, adds
    the CU base offset to adjust dw_offset.

    @param dw_attr
    The attribute the local offset was extracted from.
    @param dw_offset
    The global offset of the attribute.
    @param dw_return_offset
    The returned section (global) offset.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
    Returns DW_DLV_ERROR if the dw_attr form is not
    an offset form (for example, DW_FORM_ref_udata).
*/
DW_API int dwarf_convert_to_global_offset(Dwarf_Attribute dw_attr,
    Dwarf_Off    dw_offset,
    Dwarf_Off*   dw_return_offset,
    Dwarf_Error* dw_error);

/*! @brief Dealloc a Dwarf_Attribute
    When this call returns the dw_attr is a stale pointer.
    @param dw_attr
    The attribute to dealloc.
*/
DW_API void dwarf_dealloc_attribute(Dwarf_Attribute dw_attr);

/*! @brief Returns an array of discriminant values.

    This applies if a DW_TAG_variant has one of the
    DW_FORM_block forms.
    @see dwarf_formblock

    For an example of use and dealloc:
    @see examplediscrlist

    @param dw_dbg
    The applicable Dwarf_Debug
    @param dw_blockpointer
    The  bl_data value from a Dwarf_Block.
    @param dw_blocklen
    The  bl_len value from a Dwarf_Block.
    @param dw_dsc_head_out
    On success returns a pointer to an array
    of discriminant values in an opaque struct.
    @param dw_dsc_array_length_out
    On success returns the number of entries
    in the dw_dsc_head_out array.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_discr_list(Dwarf_Debug dw_dbg,
    Dwarf_Small    * dw_blockpointer,
    Dwarf_Unsigned   dw_blocklen,
    Dwarf_Dsc_Head * dw_dsc_head_out,
    Dwarf_Unsigned * dw_dsc_array_length_out,
    Dwarf_Error    * dw_error);

/*! @brief Access a single unsigned discriminant list entry

    It is up to the caller to know whether the discriminant
    values are signed or unsigned (therefore to know
    whether this or dwarf_discr_entry_s.
    should be called)

    @param dw_dsc
    The Dwarf_Dsc_Head applicable.
    @param dw_entrynum
    Valid values are zero to dw_dsc_array_length_out-1
    @param dw_out_type
    On success is set to either DW_DSC_label  or
    DW_DSC_range through the pointer.
    @param dw_out_discr_low
    On success set to
    the lowest in this discriminant range
    @param dw_out_discr_high
    On success set to
    the highest in this discriminant range
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_discr_entry_u(Dwarf_Dsc_Head dw_dsc,
    Dwarf_Unsigned   dw_entrynum,
    Dwarf_Half     * dw_out_type,
    Dwarf_Unsigned * dw_out_discr_low,
    Dwarf_Unsigned * dw_out_discr_high,
    Dwarf_Error    * dw_error);

/*! @brief Access to a single signed discriminant list entry

    The same as dwarf_discr_entry_u except here the values
    are signed.
*/
DW_API int dwarf_discr_entry_s(Dwarf_Dsc_Head dw_dsc,
    Dwarf_Unsigned   dw_entrynum,
    Dwarf_Half     * dw_out_type,
    Dwarf_Signed   * dw_out_discr_low,
    Dwarf_Signed   * dw_out_discr_high,
    Dwarf_Error    * dw_error);

/*! @} */

/*! @defgroup linetable CU Data-Line Table For a CU
    Access to all the line table details.
    @{
*/

/*! @brief The list of source files from the line table header

    @param dw_cu_die
    The CU DIE in this CU.
    @param dw_srcfiles
    On success allocates an array of pointers to strings
    and for each such, computes the fullest path possible
    given the CU die data for each file name listed
    in the line table header.
    @param dw_filecount
    On success returns the number of entries
    in the array of pointers to strings.
    The number returned is non-negative.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
    If there is no .debug_line[.dwo] returns DW_DLV_NO_ENTRY.

    @see examplee

*/
DW_API int dwarf_srcfiles(Dwarf_Die dw_cu_die,
    char       *** dw_srcfiles,
    Dwarf_Signed * dw_filecount,
    Dwarf_Error  * dw_error);

/*! @brief Initialize Dwarf_Line_Context for line table access

    Returns Dwarf_Line_Context pointer, needed for
    access to line table data.

    @param dw_cudie
    The Compilation Unit (CU) DIE of interest.
    @param dw_version_out
    The DWARF Line Table version number (Standard: 2,3,4, or 5)
    Version 0xf006 is an experimental (two-level) line table.
    @param dw_table_count
    Zero or one means this is a normal DWARF line table.
    Two means this is an experimental two-level line table.
    @param dw_linecontext
    On success sets the pointer to point to an opaque structure
    usable for further queries.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_srclines_b(Dwarf_Die dw_cudie,
    Dwarf_Unsigned     * dw_version_out,
    Dwarf_Small        * dw_table_count,
    Dwarf_Line_Context * dw_linecontext,
    Dwarf_Error        * dw_error);

/*! @brief Access source lines from line context

    The access to Dwarf_Line data from
    a Dwarf_Line_Context on a standard line table.

    @param dw_linecontext
    The line context of interest.
    @param dw_linebuf
    On success returns
    an array of pointers to Dwarf_Line.
    @param dw_linecount
    On success returns the count of entries
    in dw_linebuf.
    If dw_linecount is returned as zero this is
    a line table with no lines.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_srclines_from_linecontext(
    Dwarf_Line_Context dw_linecontext,
    Dwarf_Line  ** dw_linebuf,
    Dwarf_Signed * dw_linecount,
    Dwarf_Error  * dw_error);

/*! @brief  Returns line table counts and data

    Works for DWARF2,3,4,5 and for experimental
    two-level line tables. A single level table will
    have *linebuf_actuals and *linecount_actuals set
    to 0.

    Two-level line tables are non-standard and not
    documented further.
    For standard (one-level) tables, it
    will return the single table through dw_linebuf, and the
    value returned through dw_linecount_actuals will be 0.

    People not using these two-level tables
    should dwarf_srclines_from_linecontext instead.
*/
DW_API int dwarf_srclines_two_level_from_linecontext(
    Dwarf_Line_Context dw_context,
    Dwarf_Line  **   dw_linebuf ,
    Dwarf_Signed *   dw_linecount,
    Dwarf_Line  **   dw_linebuf_actuals,
    Dwarf_Signed *   dw_linecount_actuals,
    Dwarf_Error  *   dw_error);

/*! @brief Dealloc the memory allocated by dwarf_srclines_b

    The way to deallocate (free) a Dwarf_Line_Context

    @param dw_context
    The context to be deallocd (freed).
    On return the pointer passed in is stale and
    calling applications should zero the pointer.
*/
DW_API void dwarf_srclines_dealloc_b(Dwarf_Line_Context dw_context);

/*! @brief Srclines table offset

    The offset is in the relevent .debug_line or .debug_line.dwo
    section (and in a split dwarf package file includes
    the base line table offset).

    @param dw_context
    @param dw_offset
    On success returns the section offset of the
    dw_context.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_srclines_table_offset(Dwarf_Line_Context dw_context,
    Dwarf_Unsigned * dw_offset,
    Dwarf_Error  * dw_error);

/*! @brief Compilation Directory name for the CU

    Do not free() or dealloc the string,
    it is in a dwarf section.

    @param dw_context
    The Line Context of interest.
    @param dw_compilation_directory
    On success returns a pointer to a string
    identifying the compilation directory of the CU.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_srclines_comp_dir(Dwarf_Line_Context dw_context,
    const char ** dw_compilation_directory,
    Dwarf_Error * dw_error);

/*! @brief subprog count: Part of the two-level line table extension.

    A non-standard table.
    The actual meaning of subprog count left undefined here.

    @param dw_context
    The Dwarf_Line_Context of interest.
    @param dw_count
    On success returns the two-level line table subprogram
    array size in this line context.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_srclines_subprog_count(Dwarf_Line_Context dw_context,
    Dwarf_Signed * dw_count,
    Dwarf_Error  * dw_error);

/*! @brief Retrieve data from the line table subprog array

    A non-standard table. Not defined here.
    @param dw_context
    The Dwarf_Line_Context of interest.
    @param dw_index
    The item to retrieve. Valid indexes are 1 through dw_count.
    @param dw_name
    On success returns a pointer to the subprog name.
    @param dw_decl_file
    On success returns a file number through the pointer.
    @param dw_decl_line
    On success returns a line number through the pointer.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_srclines_subprog_data(Dwarf_Line_Context dw_context,
    Dwarf_Signed     dw_index,
    const char    ** dw_name,
    Dwarf_Unsigned * dw_decl_file,
    Dwarf_Unsigned * dw_decl_line,
    Dwarf_Error    * dw_error);

/*! @brief Returns values easing indexing line table file numbers.
    Count is the real count of files array entries.
    Since DWARF 2,3,4 are zero origin indexes and
    DWARF5 and later are one origin, this function
    replaces dwarf_srclines_files_count().
    @param dw_context
    The line context of interest.
    @param dw_baseindex
    On success returns the base index of valid file indexes.
    With DWARF2,3,4 the value is 1. With DWARF5 the
    value is 0.
    @param dw_count
    On success returns the real count of entries.
    @param dw_endindex
    On success returns  value such that
    callers should index as dw_baseindex through
    dw_endindex-1.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.

    @see examplec
*/
DW_API int dwarf_srclines_files_indexes(
    Dwarf_Line_Context dw_context,
    Dwarf_Signed * dw_baseindex,
    Dwarf_Signed * dw_count,
    Dwarf_Signed * dw_endindex,
    Dwarf_Error  * dw_error);

/*! @brief  Access data for each line table file.

    Has the md5ptr field so cases where DW_LNCT_MD5
    is present can return pointer to the MD5 value.
    With DWARF 5 index starts with 0.
    dwarf_srclines_files_indexes makes
    indexing through the files easy.

    @see dwarf_srclines_files_indexes
    @see examplec

    @param dw_context
    The line context of interest.
    @param dw_index_in
    The entry of interest.
    Callers should index as dw_baseindex through
    dw_endindex-1.
    @param dw_name
    If dw_name non-null
    on success returns
    The file name in the line table header
    throught the pointer.
    @param dw_directory_index
    If dw_directory_index non-null
    on success returns
    the directory number in the line table header
    through the pointer.
    @param dw_last_mod_time
    If dw_last_mod_time non-null
    on success returns
    the directory last modification date/time
    through the pointer.
    @param dw_file_length
    If dw_file_length non-null
    on success returns
    the file length recorded in the line table
    through the pointer.
    @param dw_md5ptr
    If dw_md5ptr non-null
    on success returns
    a pointer to the 16byte MD5 hash
    of the file
    through the pointer.
    If there is no md5 value present it returns 0 through
    the pointer.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.

    @see examplec
*/
DW_API int dwarf_srclines_files_data_b(
    Dwarf_Line_Context   dw_context,
    Dwarf_Signed         dw_index_in,
    const char        ** dw_name,
    Dwarf_Unsigned     * dw_directory_index,
    Dwarf_Unsigned     * dw_last_mod_time,
    Dwarf_Unsigned     * dw_file_length,
    Dwarf_Form_Data16 ** dw_md5ptr,
    Dwarf_Error        * dw_error);

/*! @brief Returns the number of include directories in the Line Table

    @param dw_line_context
    The line context of interest.
    @param dw_count
    On success returns the count of directories.
    How to use this depends on the line table version number.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.

    @see dwarf_srclines_include_dir_data
*/
DW_API int dwarf_srclines_include_dir_count(
    Dwarf_Line_Context dw_line_context,
    Dwarf_Signed * dw_count,
    Dwarf_Error  * dw_error);

/*! @brief Returns the include directories in the Line Table

    @param dw_line_context
    The line context of interest.
    @param dw_index
    Pass in an index to the line context list of include
    directories.
    If the line table is version 2,3, or 4, the
    valid indexes are 1 through dw_count.
    If the line table is version 5 the
    valid indexes are 0 through dw_count-1.
    @param dw_name
    On success it returns a pointer to a directory name.
    Do not free/deallocate the string.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.

    @see dwarf_srclines_include_dir_count
*/

DW_API int dwarf_srclines_include_dir_data(
    Dwarf_Line_Context dw_line_context,
    Dwarf_Signed    dw_index,
    const char **   dw_name,
    Dwarf_Error   * dw_error);

/*! @brief  The DWARF version number of this compile-unit
    The .debug_lines[.dwo] t
    actual tables:0 (header with no lines),
    1 (standard table), or 2 (experimental).

    @param dw_line_context
    The Line Context of interest.
    @param dw_version
    On success, returns
    the line table version through the pointer.
    @param dw_table_count
    On success, returns
    the tablecount through the pointer.
    If the table count is zero the line table is a header
    with no lines.  If the table count is 1 this is a standard
    line table.  If the table count is this is an experimental
    two-level line table.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_srclines_version(Dwarf_Line_Context dw_line_context,
    Dwarf_Unsigned * dw_version,
    Dwarf_Small    * dw_table_count,
    Dwarf_Error    * dw_error);

/*! @brief Read Line beginstatement register

    @link linetableregisters Link to Line Table Registers @endlink

    @param dw_line
    The Dwarf_Line of interest.
    @param dw_returned_bool
    On success it sets the value TRUE (if the dw_line
    has the is_stmt register set) and FALSE if is_stmt
    is not set.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_linebeginstatement(Dwarf_Line dw_line,
    Dwarf_Bool  * dw_returned_bool,
    Dwarf_Error * dw_error);

/*! @brief Read Line endsequence register flag

    @link linetableregisters Link to Line Table Registers @endlink

    @param dw_line
    The Dwarf_Line of interest.
    @param dw_returned_bool
    On success it sets the value TRUE (if the dw_line
    has the end_sequence register set) and FALSE if end_sequence
    is not set.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_lineendsequence(Dwarf_Line dw_line,
    Dwarf_Bool  * dw_returned_bool,
    Dwarf_Error * dw_error);

/*! @brief Read Line line register

    @link linetableregisters Link to Line Table Registers @endlink

    @param dw_line
    The Dwarf_Line of interest.
    @param dw_returned_linenum
    On success it sets the value to the line number
    from the Dwarf_Line line register
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_lineno(Dwarf_Line dw_line,
    Dwarf_Unsigned * dw_returned_linenum,
    Dwarf_Error    * dw_error);

/*! @brief Read Line file register

    @link linetableregisters Link to Line Table Registers @endlink

    @param dw_line
    The Dwarf_Line of interest.
    @param dw_returned_filenum
    On success it sets the value to the file number
    from the Dwarf_Line file register
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_line_srcfileno(Dwarf_Line dw_line,
    Dwarf_Unsigned * dw_returned_filenum,
    Dwarf_Error    * dw_error);

/*! @brief Is the Dwarf_Line address from DW_LNS_set_address?
    This is not a line register, but it is a flag set
    by the library in each Dwarf_Line, and it
    is derived from reading the line table.
    @param dw_line
    The Dwarf_Line of interest.
    @param dw_is_addr_set
    On success it sets the flag to TRUE or FALSE.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_line_is_addr_set(Dwarf_Line dw_line,
    Dwarf_Bool  * dw_is_addr_set,
    Dwarf_Error * dw_error);

/*! @brief Returns the address of the Dwarf_Line

    @link linetableregisters Link to Line Table Registers @endlink

    @param dw_line
    The Dwarf_Line of interest.
    @param dw_returned_addr
    On success it sets the value to the value of
    the address register in the Dwarf_Line.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_lineaddr(Dwarf_Line dw_line,
    Dwarf_Addr *     dw_returned_addr,
    Dwarf_Error*     dw_error);

/*! @brief Returns a column number through the pointer

    @link linetableregisters Link to Line Table Registers @endlink

    @param dw_line
    The Dwarf_Line of interest.
    @param dw_returned_lineoffset
    On success it sets the value to the column
    register from the Dwarf_Line.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_lineoff_b(Dwarf_Line dw_line,
    Dwarf_Unsigned * dw_returned_lineoffset,
    Dwarf_Error*     dw_error);

/*! @brief Return the file name applicable to the Dwarf_Line

    @link linetableregisters Link to Line Table Registers @endlink

    @param dw_line
    The Dwarf_Line of interest.
    @param dw_returned_name
    On success it reads the file register and finds
    the source file name from the line table header
    and returns a pointer to that file name string
    through the pointer.
    @param dw_error
    The usual error pointer.
    Do not dealloc or free the string.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_linesrc(Dwarf_Line dw_line,
    char      ** dw_returned_name,
    Dwarf_Error* dw_error);

/*! @brief Returns the basic_block line register.

    @link linetableregisters Link to Line Table Registers @endlink

    @param dw_line
    The Dwarf_Line of interest.
    @param dw_returned_bool
    On success it sets the flag to TRUE or FALSE
    from the basic_block register in the line table.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_lineblock(Dwarf_Line dw_line,
    Dwarf_Bool  *    dw_returned_bool,
    Dwarf_Error*     dw_error);

/*  We gather these into one call as it's likely one
    will want all or none of them.  */
/*! @brief Returns various line table registers in one call

    @link linetableregisters Link to Line Table Registers @endlink

    @param dw_line
    The Dwarf_Line of interest.
    @param dw_prologue_end
    On success it sets the flag to TRUE or FALSE
    from the prologue_end register in the line table.
    @param dw_epilogue_begin
    On success it sets the flag to TRUE or FALSE
    from the epilogue_begin register in the line table.
    @param dw_isa
    On success it sets the value to the value of
    from the isa register in the line table.
    @param dw_discriminator
    On success it sets the value to the value of
    from the discriminator register in the line table.
    @param dw_error
    The usual error pointer.
    @return
    DW_DLV_OK if it succeeds.
*/
DW_API int dwarf_prologue_end_etc(Dwarf_Line dw_line,
    Dwarf_Bool  *    dw_prologue_end,
    Dwarf_Bool  *    dw_epilogue_begin,
    Dwarf_Unsigned * dw_isa,
    Dwarf_Unsigned * dw_discriminator,
    Dwarf_Error *    dw_error);
/* End line table operations */

/*! @brief Experimental Two-level logical Row Number
    Experimental two level line tables. Not explained here.
    When reading from an actuals table, dwarf_line_logical()
    returns the logical row number for the line.
*/
DW_API int dwarf_linelogical(Dwarf_Line dw_line,
    Dwarf_Unsigned * dw_returned_logical,
    Dwarf_Error*     dw_error);

/*! @brief Experimental Two-level line tables call contexts
    Experimental two level line tables. Not explained here.
    When reading from a logicals table, dwarf_linecontext()
    returns the logical row number corresponding the the
    calling context for an inlined call.
*/
DW_API int dwarf_linecontext(Dwarf_Line dw_line,
    Dwarf_Unsigned * dw_returned_context,
    Dwarf_Error*     dw_error);

/*! @brief  Two-level line tables get subprogam number
    Experimental two level line tables. Not explained here.
    When reading from a logicals table, dwarf_line_subprogno()
    returns the index in the subprograms table of the inlined
    subprogram.
*/
DW_API int dwarf_line_subprogno(Dwarf_Line /*line*/,
    Dwarf_Unsigned * /*ret_subprogno*/,
    Dwarf_Error *    /*error*/);

/*! @brief Two-level line tables get subprog, file, line
    Experimental two level line tables. Not explained here.
    When reading from a logicals table, dwarf_line_subprog()
    returns the name of the inlined subprogram, its declaration
    filename, and its declaration line number, if available.
*/
DW_API int dwarf_line_subprog(Dwarf_Line /*line*/,
    char   **        /*returned_subprog_name*/,
    char   **        /*returned_filename*/,
    Dwarf_Unsigned * /*returned_lineno*/,
    Dwarf_Error *    /*error*/);

/*! @brief
    Lets the caller get detailed messages
    about some compiler errors we detect.
    Calls back, the caller should do something
    with the messages (likely just print them).
    The lines passed back already have newlines.

    @see dwarf_check_lineheader
    @see Dwarf_Printf_Callback_Info_s

    @param dw_cu_die
    The CU DIE of interest
    @param dw_error
    If DW_DLV_ERROR this shows one error encountered.
    @param dw_errcount_out
    Returns the count of detected errors through the pointer.
    @return
    DW_DLV_OK etc.
*/
DW_API int dwarf_check_lineheader_b(Dwarf_Die dw_cu_die,
    int         * dw_errcount_out,
    Dwarf_Error * dw_error);

/*! @brief Print line information in great detail.

    dwarf_print_lines lets the caller
    prints line information
    for a CU in great detail.
    Does not use printf.
    Instead it calls back to the application using a function
    pointer once per line-to-print.  The lines passed back
    already have any needed
    newlines.

    Failing to call the dwarf_register_printf_callback()
    function will prevent the lines from being passed back
    but such omission is not an error.
    the same function, but focused on checking for errors
    is
    @see dwarf_check_lineheader_b
    @see Dwarf_Printf_Callback_Info_s

    @param dw_cu_die
    The CU DIE of interest
    @param dw_error
    @param dw_errorcount_out
    @return
    DW_DLV_OK etc.

*/
DW_API int dwarf_print_lines(Dwarf_Die dw_cu_die,
    Dwarf_Error * dw_error,
    int * dw_errorcount_out);

/*! @brief For line details this records callback details

    For the structure you must fill in:
    @see Dwarf_Printf_Callback_Info_s

    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_callbackinfo
    If non-NULL pass in a pointer to your
    instance of struct Dwarf_Printf_Callback_Info_s
    with all the fields filled in.
    @return
    If dw_callbackinfo NULL it returns a copy
    of the current Dwarf_Printf_Callback_Info_s for dw_dbg.
    Otherwise it returns the previous contents of the
    struct.
*/
DW_API struct  Dwarf_Printf_Callback_Info_s
    dwarf_register_printf_callback(Dwarf_Debug dw_dbg,
    struct Dwarf_Printf_Callback_Info_s * dw_callbackinfo);

/*! @} */
/*! @defgroup ranges CU Data-Ranges data DW_AT_ranges
    In DWARF3 and DWARF4 the DW_AT_ranges attribute
    provides an offset into the .debug_ranges section.

    @see Dwarf_Ranges

    DWARF3 and DWARF4.
    DW_AT_ranges with an unsigned constant FORM (DWARF3)
    or DW_FORM_sec_offset(DWARF4).

    @{
*/
/*! @brief Access to code ranges from a CU or just
    reading through the raw .debug_ranges section.

    Adds return of the dw_realoffset to accommodate
    DWARF4 GNU split-dwarf, where the ranges could
    be in the tieddbg (meaning the real executable, a.out, not
    in a dwp). DWARF4 split-dwarf is an extension, not
    standard DWARF4.

    If printing all entries in the section pass
    in an initial dw_rangesoffset of zero and dw_die of NULL.
    Then increment dw_rangesoffset by dw_bytecount and call
    again to get the next batch of ranges.
    With a specific option dwarfdump can do this.
    This not a normal thing to do!

    @param dw_dbg
    The Dwarf_Debug of interest
    @param dw_rangesoffset
    The offset to read from in the section.
    @param dw_die
    Pass in the DIE whose DW_AT_ranges brought us to ranges.
    @param dw_return_realoffset
    The actual offset in the section actually read.
    In a tieddbg this
    @param dw_rangesbuf
    A pointer to an array of structs is returned here.
    @param dw_rangecount
    The count of structs in the array is returned here.
    @param dw_bytecount
    The number of bytes in the .debug_ranges section
    applying to the returned array. This makes possible
    just marching through the section by offset.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_ranges_b(Dwarf_Debug dw_dbg,
    Dwarf_Off        dw_rangesoffset,
    Dwarf_Die        dw_die,
    Dwarf_Off *      dw_return_realoffset,
    Dwarf_Ranges **  dw_rangesbuf,
    Dwarf_Signed *   dw_rangecount,
    Dwarf_Unsigned * dw_bytecount,
    Dwarf_Error *    dw_error);

/*! @brief dealloc the array dw_rangesbuf

    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_rangesbuf
    The dw_rangesbuf pointer returned by dwarf_get_ranges_b
    @param dw_rangecount
    The dw_rangecount returned by dwarf_get_ranges_b
*/
DW_API void dwarf_dealloc_ranges(Dwarf_Debug dw_dbg,
    Dwarf_Ranges * dw_rangesbuf,
    Dwarf_Signed   dw_rangecount);
/*! @} */

/*! @defgroup rnglists CU Data Rnglists .debug_rnglists DWARF5

    Used in DWARF5.
    DW_FORM_rnglistx
    DW_AT_ranges with DW_FORM_sec_offset
    @{
*/

/*! @brief Get Access to DWARF5 rnglists

    Opens a Dwarf_Rnglists_Head to access a set of
    DWARF5 rangelists .debug_rnglists
    DW_FORM_sec_offset DW_FORM_rnglistx
    (DW_AT_ranges in DWARF5).

    @param dw_attr
    The attribute referring to .debug_rnglists
    @param dw_theform
    The form number.
    @param dw_index_or_offset_value
    If the form is an index, pass it here.
    If the form is an offset, pass that here.
    @param dw_head_out
    On success creates a record owning the
    rnglists data for this attribute.
    @param dw_count_of_entries_in_head
    On success this is set to the number of
    entry in the rnglists for this attribute.
    @param dw_global_offset_of_rle_set
    On success set to the global offset of the rnglists
    in the rnglists section.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.

*/
DW_API int dwarf_rnglists_get_rle_head(Dwarf_Attribute dw_attr,
    Dwarf_Half            dw_theform,
    Dwarf_Unsigned        dw_index_or_offset_value,
    Dwarf_Rnglists_Head * dw_head_out,
    Dwarf_Unsigned *      dw_count_of_entries_in_head,
    Dwarf_Unsigned *      dw_global_offset_of_rle_set,
    Dwarf_Error    *      dw_error);

/*! @brief access rnglist entry details.

    @param dw_head
    The Dwarf_Rnglists_Head of interest.
    @param dw_entrynum
    Valid values are 0 through dw_count_of_entries_in_head-1.
    @param dw_entrylen
    On success returns
    the length in bytes of this individual entry.
    @param dw_rle_value_out
    On success returns
    the RLE value of the entry, such as DW_RLE_startx_endx.
    This determines which of dw_raw1 and dw_raw2
    contain meaningful data.
    @param dw_raw1
    On success returns
    a value directly recorded in the rangelist entry
    if that applies to this rle.
    @param dw_raw2
    On success returns
    a value directly recorded in the rangelist entry
    if that applies to this rle.
    @param dw_debug_addr_unavailable
    On success returns a flag.
    If the .debug_addr section is required but absent or
    unavailable the flag is set to TRUE.
    Otherwise sets the flag FALSE.
    @param dw_cooked1
    On success returns (if appropriate) the
    dw_raw1 value turned into a valid address.
    @param dw_cooked2
    On success returns (if appropriate) the
    dw_raw2 value turned into a valid address.
    Ignore the value if dw_debug_addr_unavailable is set.
    @param dw_error
    The usual error detail return pointer.
    Ignore the value if dw_debug_addr_unavailable is set.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_rnglists_entry_fields_a(
    Dwarf_Rnglists_Head dw_head,
    Dwarf_Unsigned   dw_entrynum,
    unsigned int   * dw_entrylen,
    unsigned int   * dw_rle_value_out,
    Dwarf_Unsigned * dw_raw1,
    Dwarf_Unsigned * dw_raw2,
    Dwarf_Bool     * dw_debug_addr_unavailable,
    Dwarf_Unsigned * dw_cooked1,
    Dwarf_Unsigned * dw_cooked2,
    Dwarf_Error *    dw_error);

/*! @brief Dealloc a Dwarf_Rnglists_Head

    @param dw_head
    dealloc all the memory associated with dw_head.
    The caller should then immediately set
    the pointer to zero/NULL as it is stale.
*/
DW_API void dwarf_dealloc_rnglists_head(Dwarf_Rnglists_Head dw_head);

/*! @brief Loads all .debug_rnglists headers.

    Loads all the rnglists headers and
    returns DW_DLV_NO_ENTRY if the section
    is missing or empty.
    Intended to be done quite early.
    It is automatically done if anything
    needing CU or DIE information is called,
    so it is not necessary for you to call this
    in any normal situation.

    Doing it more than once is never necessary
    or harmful. There is no deallocation call
    made visible, deallocation happens
    when dwarf_finish is called.

    @param dw_dbg
    @param dw_rnglists_count
    On success it returns the number of
    rnglists headers in the section through
    dw_rnglists_count.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
    If the section does not exist the function returns
    DW_DLV_OK.
*/
DW_API int dwarf_load_rnglists(Dwarf_Debug dw_dbg,
    Dwarf_Unsigned * dw_rnglists_count,
    Dwarf_Error    * dw_error);

/*! @brief Retrieve the section offset of a rnglist.

    Can be used to access raw rnglist data.
    Not used by most callers.
    See DWARF5 Section 7.28 Range List Table Page 242

    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_context_index
    Begin this at zero.
    @param dw_offsetentry_index
    Begin this at zero.
    @param dw_offset_value_out
    On success returns the rangelist entry
    offset within the rangelist set.
    @param dw_global_offset_value_out
    On success returns the rangelist entry
    offset within rnglist section.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
    If there are no rnglists at all, or
    if one of the above index values is too high
    to be valid it returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_get_rnglist_offset_index_value(Dwarf_Debug dw_dbg,
    Dwarf_Unsigned   dw_context_index,
    Dwarf_Unsigned   dw_offsetentry_index,
    Dwarf_Unsigned * dw_offset_value_out,
    Dwarf_Unsigned * dw_global_offset_value_out,
    Dwarf_Error    * dw_error);

/*! @brief Access to internal data on rangelists.

    Returns detailed data from a Dwarf_Rnglists_Head
    Since this is primarily internal data we don't
    describe the details of the returned fields here.
*/
DW_API int dwarf_get_rnglist_head_basics(Dwarf_Rnglists_Head dw_head,
    Dwarf_Unsigned * dw_rle_count,
    Dwarf_Unsigned * dw_rnglists_version,
    Dwarf_Unsigned * dw_rnglists_index_returned,
    Dwarf_Unsigned * dw_bytes_total_in_rle,
    Dwarf_Half     * dw_offset_size,
    Dwarf_Half     * dw_address_size,
    Dwarf_Half     * dw_segment_selector_size,
    Dwarf_Unsigned * dw_overall_offset_of_this_context,
    Dwarf_Unsigned * dw_total_length_of_this_context,
    Dwarf_Unsigned * dw_offset_table_offset,
    Dwarf_Unsigned * dw_offset_table_entrycount,
    Dwarf_Bool     * dw_rnglists_base_present,
    Dwarf_Unsigned * dw_rnglists_base,
    Dwarf_Bool     * dw_rnglists_base_address_present,
    Dwarf_Unsigned * dw_rnglists_base_address,
    Dwarf_Bool     * dw_rnglists_debug_addr_base_present,
    Dwarf_Unsigned * dw_rnglists_debug_addr_base,
    Dwarf_Error    * dw_error);

/*! @brief Access to rnglists header data

    This returns, independent of any DIEs or CUs
    information on the .debug_rnglists headers
    present in the section.

    We do not document the details here.
    See the DWARF5 standard.

    Enables printing of details about the Range List Table
    Headers, one header per call. Index starting at 0.
    Returns DW_DLV_NO_ENTRY if index is too high for the table.
    A .debug_rnglists section may contain any number
    of Range List Table Headers with their details.
*/
DW_API int dwarf_get_rnglist_context_basics(Dwarf_Debug dw_dbg,
    Dwarf_Unsigned   dw_index,
    Dwarf_Unsigned * dw_header_offset,
    Dwarf_Small  *   dw_offset_size,
    Dwarf_Small  *   dw_extension_size,
    unsigned int *   dw_version,
    Dwarf_Small  *   dw_address_size,
    Dwarf_Small  *   dw_segment_selector_size,
    Dwarf_Unsigned * dw_offset_entry_count,
    Dwarf_Unsigned * dw_offset_of_offset_array,
    Dwarf_Unsigned * dw_offset_of_first_rangeentry,
    Dwarf_Unsigned * dw_offset_past_last_rangeentry,
    Dwarf_Error *    dw_error);

/*! @brief Access to raw rnglists range data

    Describes the actual raw data recorded in a particular
    range entry.

    We do not describe all these fields for now.
*/
DW_API int dwarf_get_rnglist_rle(Dwarf_Debug dw_dbg,
    Dwarf_Unsigned dw_contextnumber,
    Dwarf_Unsigned dw_entry_offset,
    Dwarf_Unsigned dw_endoffset,
    unsigned int   * dw_entrylen,
    unsigned int   * dw_entry_kind,
    Dwarf_Unsigned * dw_entry_operand1,
    Dwarf_Unsigned * dw_entry_operand2,
    Dwarf_Error    * dw_error);
/*! @} */
/*! @defgroup locations CU Data- Data Locations DWARF2-DWARF5
    @{
*/
/*! @brief Location Lists and Expressions

    @param dw_attr
    The attribute must refer to a location expression
    or a location list, so must be DW_FORM_block,
    DW_FORM_exprloc, or a loclist reference form..
    @param dw_loclist_head
    On success returns a pointer to the created
    loclist head record.
    @param dw_locentry_count
    On success returns the count of records.
    For an expression it will be one.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_loclist_c(Dwarf_Attribute dw_attr,
    Dwarf_Loc_Head_c * dw_loclist_head,
    Dwarf_Unsigned   * dw_locentry_count,
    Dwarf_Error      * dw_error);

#define DW_LKIND_expression   0 /* DWARF2,3,4,5 */
#define DW_LKIND_loclist      1 /* DWARF 2,3,4 */
#define DW_LKIND_GNU_exp_list 2 /* GNU DWARF4 .dwo extension */
#define DW_LKIND_loclists     5 /* DWARF5 loclists */
#define DW_LKIND_unknown     99

/*! @brief Know what kind of location data it is

    @param dw_loclist_head
    Pass in a loclist head pointer.
    @param dw_lkind
    On success returns the loclist kind
    through the pointer. For example DW_LKIND_expression.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_loclist_head_kind(
    Dwarf_Loc_Head_c dw_loclist_head,
    unsigned int  * dw_lkind,
    Dwarf_Error   * dw_error);

/*! @brief Retrieve the details of a location expression

    Cooked value means the addresses from the location
    description after base values applied, so they
    are actual addresses.
    debug_addr_unavailable non-zero means the record from a
    Split Dwarf skeleton unit could not be accessed from
    the .dwo section or dwp object so the
    cooked values could not be calculated.

    @param dw_loclist_head
    A loclist head pointer.
    @param dw_index
    Pass in an index value less than dw_locentry_count .
    @param dw_lle_value_out
    On success returns the DW_LLE value applicable, such
    as DW_LLE_start_end .
    @param dw_rawlowpc
    On success returns the first operand in the expression
    (if the expression has an operand).
    @param dw_rawhipc
    On success returns the second operand in the expression.
    (if the expression has a second operand).
    @param dw_debug_addr_unavailable
    On success returns FALSE if the data required to calculate
    dw_lowpc_cooked or dw_hipc_cooked was present
    or TRUE if some required
    data was missing (for example in split dwarf).
    @param dw_lowpc_cooked
    On success and if dw_debug_addr_unavailable FALSE
    returns the true low address.
    @param dw_hipc_cooked
    On success and if dw_debug_addr_unavailable FALSE
    returns the true high address.
    @param dw_locexpr_op_count_out
    On success returns the count of operations
    in the expression.
    @param dw_locentry_out
    On success returns a pointer to a specific location
    description.
    @param dw_loclist_source_out
    On success returns the applicable DW_LKIND value.
    @param dw_expression_offset_out
    On success returns the offset of the expression
    in the applicable section.
    @param dw_locdesc_offset_out
    On return sets the offset to the location description
    offset (if that is meaningful) or zero for
    simple location expressions.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_locdesc_entry_d(Dwarf_Loc_Head_c dw_loclist_head,
    Dwarf_Unsigned    dw_index,
    Dwarf_Small    *  dw_lle_value_out,
    Dwarf_Unsigned *  dw_rawlowpc,
    Dwarf_Unsigned *  dw_rawhipc,
    Dwarf_Bool     *  dw_debug_addr_unavailable,
    Dwarf_Addr     *  dw_lowpc_cooked,
    Dwarf_Addr     *  dw_hipc_cooked,
    Dwarf_Unsigned *  dw_locexpr_op_count_out,
    Dwarf_Locdesc_c * dw_locentry_out,
    Dwarf_Small    *  dw_loclist_source_out,
    Dwarf_Unsigned *  dw_expression_offset_out,
    Dwarf_Unsigned *  dw_locdesc_offset_out,
    Dwarf_Error    *  dw_error);

/*! @brief Get the raw values from a single location operation

    Some of the following (DW_raw?) appear completly
    pointless - a mistake.

    @param dw_locdesc
    Pass in a valid Dwarf_Locdesc_c.
    @param dw_index
    Pass in the operator index. zero through
    dw_locexpr_op_count_out-1.
    @param dw_operator_out
    On success returns the DW_OP operator, such as DW_OP_plus .
    @param dw_operand1
    On success returns the value of the operand or zero.
    @param dw_operand2
    On success returns the value of the operand or zero.
    @param dw_operand3
    On success returns the value of the operand or zero.
    @param dw_rawop1
    Identical to dw_operand1
    @param dw_rawop2
    Identical to dw_operand2
    @param dw_rawop3
    Identical to dw_operand3
    @param dw_offset_for_branch
    On success returns
    The byte offset of the operator within the
    entire expression.  Useful for checking the correctness
    of operators that branch..
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_location_op_value_d(Dwarf_Locdesc_c dw_locdesc,
    Dwarf_Unsigned   dw_index,
    Dwarf_Small    * dw_operator_out,
    Dwarf_Unsigned * dw_operand1,
    Dwarf_Unsigned * dw_operand2,
    Dwarf_Unsigned * dw_operand3,
    Dwarf_Unsigned * dw_rawop1,
    Dwarf_Unsigned * dw_rawop2,
    Dwarf_Unsigned * dw_rawop3,
    Dwarf_Unsigned * dw_offset_for_branch,
    Dwarf_Error*     dw_error);
/*! @brief Generate a Dwarf_Loc_Head_c from an expression block

    Useful if you have an expression block (from somewhere),
    do not have a Dwarf_Attribute available,
    and wish to deal with the expression.

    @param dw_dbg
    The applicable Dwarf_Debug
    @param dw_expression_in
    Pass in a pointer to the expression bytes.
    @param dw_expression_length
    Pass in the length, in bytes, of the expression.
    @param dw_address_size
    Pass in the applicable address_size.
    @param dw_offset_size
    Pass in the applicable offset size.
    @param dw_dwarf_version
    Pass in the applicable dwarf version.
    @param dw_loc_head
    On success returns a pointer to a dwarf location
    head record for use in getting to the details
    of the expression.
    @param dw_listlen
    On success, sets the listlen to one.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_loclist_from_expr_c(Dwarf_Debug dw_dbg,
    Dwarf_Ptr      dw_expression_in,
    Dwarf_Unsigned dw_expression_length,
    Dwarf_Half     dw_address_size,
    Dwarf_Half     dw_offset_size,
    Dwarf_Small    dw_dwarf_version,
    Dwarf_Loc_Head_c* dw_loc_head,
    Dwarf_Unsigned  * dw_listlen,
    Dwarf_Error     * dw_error);

/*! @brief frees all memory allocated for Dwarf_Loc_Head_c
    @param dw_head
    A head pointer
*/
DW_API void dwarf_loc_head_c_dealloc(Dwarf_Loc_Head_c dw_head);

/*  These interfaces allow reading the .debug_loclists
    section. Independently of DIEs.
    Normal use of .debug_loclists uses
    dwarf_get_loclist_c() to open access to any kind of location
    or loclist and uses dwarf_loc_head_c_dealloc() to
    deallocate that memory once one is finished with
    that data. So for most purposes you do not need
    to use these functions
    See dwarf_get_loclist_c() to open a Dwarf_Loc_Head_c
    on any type of location list or expression. */

/*  Loads all the loclists headers and
    returns DW_DLV_NO_ENTRY if the section
    is missing or empty.
    Intended to be done quite early and
    it is automatically
    done if .debug_info is loaded.
    Doing it more than once is never necessary
    or harmful. There is no deallocation call
    made visible, deallocation happens
    when dwarf_finish() is called.
    With DW_DLV_OK it returns the number of
    loclists headers in the section through
    loclists_count. */
/*! @brief Load Loclists

    This loads .debug_loclists (DWARF5).
    It is unlikely you have a reason to use this function.
    If CUs or DIES  have been referenced in any way
    loading is already done. A duplicate loading
    attempt returns DW_DLV_OK immediately, returning
    dw_loclists_count filled in and does nothing else.

    @param dw_dbg
    The applicable Dwarf_Debug.
    @param dw_loclists_count
    On success, returns the number of
    DWARF5 loclists contexts in the section,
    whether this is the first or a duplicate
    load.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK if it loaded successfully
    or if it is a duplicate load.
    If no .debug_loclists
    present returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_load_loclists(Dwarf_Debug dw_dbg,
    Dwarf_Unsigned * dw_loclists_count,
    Dwarf_Error    * dw_error);

/*! @brief Return certain loclists offsets

    Useful with the DWARF5 .debug_loclists section.

    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_context_index
    Pass in the loclists context index.
    @param dw_offsetentry_index
    Pass in the offset array index.
    @param dw_offset_value_out
    On success returns the offset value at
    offset table[dw_offsetentry_index],
    an offset local to this context.
    @param dw_global_offset_value_out
    On success returns the same offset value
    but with the offset of the table added in
    to form a section offset.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
    If one of the indexes passed in is out of range
    it returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_get_loclist_offset_index_value(Dwarf_Debug dw_dbg,
    Dwarf_Unsigned   dw_context_index,
    Dwarf_Unsigned   dw_offsetentry_index,
    Dwarf_Unsigned * dw_offset_value_out,
    Dwarf_Unsigned * dw_global_offset_value_out,
    Dwarf_Error    * dw_error);

/*! @brief  Return basic data about a loclists head

    Used by dwarfdump to print basic data from the
    data generated to look at a specific loclist
    context as returned by
    dwarf_loclists_index_get_lle_head()
    or
    dwarf_loclists_offset_get_lle_head.
    Here we know there was a Dwarf_Attribute so
    additional things are known as compared to calling
    dwarf_get_loclist_context_basics
    See DWARF5 Section 7.20 Location List Table
    page 243.
*/
DW_API int dwarf_get_loclist_head_basics(Dwarf_Loc_Head_c dw_head,
    Dwarf_Small    * dw_lkind,
    Dwarf_Unsigned * dw_lle_count,
    Dwarf_Unsigned * dw_loclists_version,
    Dwarf_Unsigned * dw_loclists_index_returned,
    Dwarf_Unsigned * dw_bytes_total_in_rle,
    Dwarf_Half     * dw_offset_size,
    Dwarf_Half     * dw_address_size,
    Dwarf_Half     * dw_segment_selector_size,
    Dwarf_Unsigned * dw_overall_offset_of_this_context,
    Dwarf_Unsigned * dw_total_length_of_this_context,
    Dwarf_Unsigned * dw_offset_table_offset,
    Dwarf_Unsigned * dw_offset_table_entrycount,
    Dwarf_Bool     * dw_loclists_base_present,
    Dwarf_Unsigned * dw_loclists_base,
    Dwarf_Bool     * dw_loclists_base_address_present,
    Dwarf_Unsigned * dw_loclists_base_address,
    Dwarf_Bool     * dw_loclists_debug_addr_base_present,
    Dwarf_Unsigned * dw_loclists_debug_addr_base,
    Dwarf_Unsigned * dw_offset_this_lle_area,
    Dwarf_Error    * dw_error);

/*! @brief  Return basic data about a loclists context

    Some of the same values as from
    dwarf_get_loclist_head_basics
    but here without any dependence on data
    drived from a CU context.
    Useful to print raw loclist data.
*/
DW_API int dwarf_get_loclist_context_basics(Dwarf_Debug dw_dbg,
    Dwarf_Unsigned   dw_index,
    Dwarf_Unsigned * dw_header_offset,
    Dwarf_Small    * dw_offset_size,
    Dwarf_Small    * dw_extension_size,
    unsigned int   * dw_version,
    Dwarf_Small    * dw_address_size,
    Dwarf_Small    * dw_segment_selector_size,
    Dwarf_Unsigned * dw_offset_entry_count,
    Dwarf_Unsigned * dw_offset_of_offset_array,
    Dwarf_Unsigned * dw_offset_of_first_locentry,
    Dwarf_Unsigned * dw_offset_past_last_locentry,
    Dwarf_Error    * dw_error);

/*! @brief  Return basic data about a loclists context entry

    Useful to print raw loclist data.
*/
DW_API int dwarf_get_loclist_lle( Dwarf_Debug dw_dbg,
    Dwarf_Unsigned   dw_contextnumber,
    Dwarf_Unsigned   dw_entry_offset,
    Dwarf_Unsigned   dw_endoffset,
    unsigned int   * dw_entrylen,
    unsigned int   * dw_entry_kind,
    Dwarf_Unsigned * dw_entry_operand1,
    Dwarf_Unsigned * dw_entry_operand2,
    Dwarf_Unsigned * dw_expr_ops_blocksize,
    Dwarf_Unsigned * dw_expr_ops_offset,
    Dwarf_Small   ** dw_expr_opsdata,
    Dwarf_Error    * dw_error);
/*! @} */

/*! @defgroup macro CU Data-Macro .debug_macro DWARF5 data access

    @link examplep5 An example reading .debug_macro @endlink
    @{
*/

/*! @brief DWARF5 .debug_macro access via Dwarf_Die

    @param dw_die
    The CU DIE of interest.
    @param dw_version_out
    On success returns the macro context version (5)
    @param dw_macro_context
    On success returns a pointer to a macro context
    which allows access to the context content.
    @param dw_macro_unit_offset_out
    On success returns the offset of the macro context.
    @param dw_macro_ops_count_out
    On success returns the number of macro operations
    in the context.
    @param dw_macro_ops_data_length_out
    On success returns the length in bytes of the
    operations in the context.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
    If no .debug_macro section exists for the
    CU it returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_get_macro_context(Dwarf_Die dw_die,
    Dwarf_Unsigned      * dw_version_out,
    Dwarf_Macro_Context * dw_macro_context,
    Dwarf_Unsigned      * dw_macro_unit_offset_out,
    Dwarf_Unsigned      * dw_macro_ops_count_out,
    Dwarf_Unsigned      * dw_macro_ops_data_length_out,
    Dwarf_Error         * dw_error);

/*! @brief DWARF5 .debug_macro access via Dwarf_Die and an offset

    @param dw_die
    The CU DIE of interest.
    @param dw_offset
    The offset in the section to begin reading.
    @param dw_version_out
    On success returns the macro context version (5)
    @param dw_macro_context
    On success returns a pointer to a macro context
    which allows access to the context content.
    @param dw_macro_ops_count_out
    On success returns the number of macro operations
    in the context.
    @param dw_macro_ops_data_length
    On success returns the length in bytes of the
    macro context, starting at the offset of the first
    byte of the context.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
    If no .debug_macro section exists for the
    CU it returns DW_DLV_NO_ENTRY.
    If the dw_offset is outside the section it
    returns DW_DLV_ERROR.
*/
DW_API int dwarf_get_macro_context_by_offset(Dwarf_Die dw_die,
    Dwarf_Unsigned        dw_offset,
    Dwarf_Unsigned      * dw_version_out,
    Dwarf_Macro_Context * dw_macro_context,
    Dwarf_Unsigned      * dw_macro_ops_count_out,
    Dwarf_Unsigned      * dw_macro_ops_data_length,
    Dwarf_Error         * dw_error);

/*  New December 2020. Sometimes its necessary to know
    a context total length including macro 5 header */
/*! @brief Return a macro context total length

    @param dw_context
    A pointer to the macro context of interest.
    @param dw_mac_total_len
    On success returns the length in bytes of
    the macro context.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_macro_context_total_length(
    Dwarf_Macro_Context dw_context,
    Dwarf_Unsigned * dw_mac_total_len,
    Dwarf_Error    * dw_error);

/*! @brief Dealloc a macro context

    @param dw_mc
    A pointer to the macro context of interest.
    On return the caller should zero the pointer
    as the pointer is then stale.
*/
DW_API void dwarf_dealloc_macro_context(Dwarf_Macro_Context dw_mc);

/*! @brief Access the internal details of a Dwarf_Macro_Context

    Not described in detail here. See DWARF5 Standard
    Section 6.3.1 Macro Information Header page 166.
*/
DW_API int dwarf_macro_context_head(Dwarf_Macro_Context dw_mc,
    Dwarf_Half     * dw_version,
    Dwarf_Unsigned * dw_mac_offset,
    Dwarf_Unsigned * dw_mac_len,
    Dwarf_Unsigned * dw_mac_header_len,
    unsigned int   * dw_flags,
    Dwarf_Bool     * dw_has_line_offset,
    Dwarf_Unsigned * dw_line_offset,
    Dwarf_Bool     * dw_has_offset_size_64,
    Dwarf_Bool     * dw_has_operands_table,
    Dwarf_Half     * dw_opcode_count,
    Dwarf_Error    * dw_error);

/*! @brief Access to the details of the opcode operands table

    Not of much interest to most libdwarf users.

    @param dw_mc
    The macro context of interest.
    @param dw_index
    The opcode operands table index. 0 through dw_opcode_count-1.
    @param dw_opcode_number
    On success returns the opcode number in the table.
    @param dw_operand_count
    On success returns the number of forms
    for that dw_index.
    @param dw_operand_array
    On success returns the array of op operand
    forms
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.

*/
DW_API int dwarf_macro_operands_table(Dwarf_Macro_Context dw_mc,
    Dwarf_Half           dw_index, /* 0 to opcode_count -1 */
    Dwarf_Half         * dw_opcode_number,
    Dwarf_Half         * dw_operand_count,
    const Dwarf_Small ** dw_operand_array,
    Dwarf_Error        * dw_error);

/*! @brief Access macro operation details of a single operation

    Useful for printing basic data about the operation.

    @param dw_macro_context
    The macro context of interest.
    @param dw_op_number
    valid values are 0 through dw_macro_ops_count_out-1.
    @param dw_op_start_section_offset
    On success returns the section offset of this
    operator.
    @param dw_macro_operator
    On success returns the the macro operator itself,
    for example DW_MACRO_define.
    @param dw_forms_count
    On success returns the numer of forms in the formcode array.
    @param dw_formcode_array
    On success returns a pointer to the formcode array
    of operand forms.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_macro_op(Dwarf_Macro_Context dw_macro_context,
    Dwarf_Unsigned       dw_op_number,
    Dwarf_Unsigned     * dw_op_start_section_offset,
    Dwarf_Half         * dw_macro_operator,
    Dwarf_Half         * dw_forms_count,
    const Dwarf_Small ** dw_formcode_array,
    Dwarf_Error        * dw_error);

/*! @brief Get Macro defundef

    To extract the value portion of a macro define:
    @see dwarf_find_macro_value_start

    @param dw_macro_context
    The macro context of interest.
    @param dw_op_number
    valid values are 0 through dw_macro_ops_count_out-1.
    The op number must be for a def/undef.
    @param dw_line_number
    The line number in the user source for this define/undef
    @param dw_index
    On success if the macro is an strx form
    the value returned
    is the string index in the record,
    otherwise zero is returned.
    @param dw_offset
    On success if the macro is an strp or sup form the value
    returned is the string offset in the appropriate section,
    otherwise zero is returned.
    @param dw_forms_count
    On success the value 2 is returned.
    @param dw_macro_string
    On success a pointer to a null-terminated
    string is returned.
    Do not dealloc or free this string.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
    It is an error if operator dw_op_number is not a
    DW_MACRO_define, DW_MACRO_undef, DW_MACRO_define_strp
    DW_MACRO_undef_strp, DW_MACRO_undef_sup,
    DW_MACRO_undef_sup,
    DW_MACRO_define_strx, or DW_MACRO_undef_strx,
*/
DW_API int dwarf_get_macro_defundef(
    Dwarf_Macro_Context dw_macro_context,
    Dwarf_Unsigned   dw_op_number,
    Dwarf_Unsigned * dw_line_number,
    Dwarf_Unsigned * dw_index,
    Dwarf_Unsigned * dw_offset,
    Dwarf_Half     * dw_forms_count,
    const char    ** dw_macro_string,
    Dwarf_Error    * dw_error);

/*! @brief Get Macro start end

    @param dw_macro_context
    The macro context of interest.
    @param dw_op_number
    Valid values are 0 through dw_macro_ops_count_out-1.
    The op number must be for a start/end.
    @param dw_line_number
    If end_file nothing is returned here.
    If start_file on success returns the line number
    of the source line of the include direcive.
    @param dw_name_index_to_line_tab
    If end_file nothing is returned here.
    If start_file on success returns the file name
    index in the line table file names table.
    @param dw_src_file_name
    If end_file nothing is returned here.
    If start_file on success returns a pointer to
    the null-terminated source file name.
    Do not free or dealloc this string.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
    It is an error if the operator is not
    DW_MACRO_start_file or DW_MACRO_end_file.
*/
DW_API int dwarf_get_macro_startend_file(
    Dwarf_Macro_Context dw_macro_context,
    Dwarf_Unsigned   dw_op_number,
    Dwarf_Unsigned * dw_line_number,
    Dwarf_Unsigned * dw_name_index_to_line_tab,
    const char    ** dw_src_file_name,
    Dwarf_Error    * dw_error);

/*! @brief Get Macro import

    @param dw_macro_context
    The macro context of interest.
    @param dw_op_number
    Valid values are 0 through dw_macro_ops_count_out-1.
    @param dw_target_offset
    Returns the offset in the imported section.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
    It is an error if the operator is not
    DW_MACRO_import or DW_MACRO_import_sup.
*/
DW_API int dwarf_get_macro_import(
    Dwarf_Macro_Context dw_macro_context,
    Dwarf_Unsigned   dw_op_number,
    Dwarf_Unsigned * dw_target_offset,
    Dwarf_Error    * dw_error);
/*! @} */

/*! @defgroup macinfo CU Data-Macinfo DWARF2-4 data access

    Reading the .debug_macinfo section.

    The section is rarely used since it takes a lot
    of disk space. DWARF5 has much more compact
    macro data (in  section .debug_macro).

    For an example see
    @link examplep2 An example reading .debug_macinfo @endlink

    @{

*/
/*! @brief returns a pointer to the value part of a macro

    This function Works for all versions, DWARF2-DWARF5

    @param dw_macro_string
    The macro string passed in should be properly formatted
    with a name, a space, and then the value portion (whether
    a function-like macro or not function-like).
    @returns
    On success it returns a pointer to the value portion
    of the macro.  On failure it returns a pointer
    to a NUL byte (so a zero-length string).
*/
DW_API char* dwarf_find_macro_value_start(char * dw_macro_string);

/*! @brief Getting .debug_macinfo macro details.

    @link examplep2 An example calling this function @endlink

    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_macro_offset
    The offset in the section you wish to start from.
    @param dw_maximum_count
    Pass in a count to ensure we will not allocate
    an excessive amount (guarding against a

    @param dw_entry_count
    On success returns a count of the macro operations
    in a CU macro set.
    @param dw_details
    On success returns a pointer to an array of
    struct DW_Macro_Details_s .
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_macro_details(Dwarf_Debug dw_dbg,
    Dwarf_Off              dw_macro_offset,
    Dwarf_Unsigned         dw_maximum_count,
    Dwarf_Signed         * dw_entry_count,
    Dwarf_Macro_Details ** dw_details,
    Dwarf_Error *          dw_error);

/*! @} */

/*! @defgroup frame Frame .debug_frame and .eh_frame Access
    @{
*/

/*! @brief Get lists of .debug_frame FDEs and CIEs

    See DWARF5 Section 6.4 Call Frame Information,
    page 171.

    For an example see
    @link exampleq Example getting FDE CIE lists @endlink

    The FDE array returned through dw_fde_data
    is sorted low-to-high  by the lowest-pc in each FDE.
    
    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_cie_data
    On success
    returns a pointer to an array of pointers to CIE data.
    @param dw_cie_element_count
    On success returns a count of the number of elements
    in the dw_cie_data array.
    @param dw_fde_data
    On success
    returns a pointer to an array of pointers to FDE data.
    @param dw_fde_element_count
    On success returns a count of the number of elements
    in the dw_fde_data array.
    On success
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_fde_list(Dwarf_Debug dw_dbg,
    Dwarf_Cie**      dw_cie_data,
    Dwarf_Signed*    dw_cie_element_count,
    Dwarf_Fde**      dw_fde_data,
    Dwarf_Signed*    dw_fde_element_count,
    Dwarf_Error*     dw_error);
/*! @brief Get lists of .eh_frame FDEs and CIEs

    The arguments are identical to the previous
    function, the difference is the section read.
    The GNU-defined .eh_frame section is very similar to
    .debug_frame but has unique features that
    matter when following a stack trace.
    @see dwarf_get_fde_list
*/
DW_API int dwarf_get_fde_list_eh(Dwarf_Debug dw_dbg,
    Dwarf_Cie**      dw_cie_data,
    Dwarf_Signed*    dw_cie_element_count,
    Dwarf_Fde**      dw_fde_data,
    Dwarf_Signed*    dw_fde_element_count,
    Dwarf_Error*     dw_error);

/*! @brief Release storage associated with FDE and CIE arrays

    Applies to .eh_frame and .debug_frame
    lists.

    @param dw_dbg
    The Dwarf_Debug used in the list setup.
    @param dw_cie_data
    As returned from the list setup call.
    @param dw_cie_element_count
    @param dw_fde_data
    As returned from the list setup call.
    @param dw_fde_element_count
    As returned from the list setup call.

    On return the pointers passed in dw_cie_data
    and dw_fde_data should be zeroed by the
    caller as they are then stale pointers.
*/
DW_API void dwarf_dealloc_fde_cie_list(Dwarf_Debug dw_dbg,
    Dwarf_Cie *  dw_cie_data,
    Dwarf_Signed dw_cie_element_count,
    Dwarf_Fde *  dw_fde_data,
    Dwarf_Signed dw_fde_element_count);

/*! @brief Returns the FDE data for a single FDE

    @param dw_fde
    The FDE of interest.
    @param dw_low_pc
    On success
    returns the low pc value for the function involved.
    @param dw_func_length
    On success returns the length of the function
    code in bytes.
    @param dw_fde_bytes
    On success 
    returns a pointer to the bytes of the FDE.
    @param dw_fde_byte_length
    On success returns the length of the 
    dw_fde_bytes area.
    @param dw_cie_offset
    On success returns the section offset of the associated CIE. 
    @param dw_cie_index
    On success returns the CIE index of the associated CIE.
    @param dw_fde_offset
    On success returns the section offset of this FDE.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_fde_range(Dwarf_Fde dw_fde,
    Dwarf_Addr*      dw_low_pc,
    Dwarf_Unsigned*  dw_func_length,
    Dwarf_Small    **dw_fde_bytes,
    Dwarf_Unsigned*  dw_fde_byte_length,
    Dwarf_Off*       dw_cie_offset,
    Dwarf_Signed*    dw_cie_index,
    Dwarf_Off*       dw_fde_offset,
    Dwarf_Error*     dw_error);

/*! @brief IRIX only access to C++ destructor tables

    This applies only to IRIX C++ destructor information
    which was never documented and is unlikely to be of interest.
*/
DW_API int dwarf_get_fde_exception_info(Dwarf_Fde dw_fde,
    Dwarf_Signed*    dw_offset_into_exception_tables,
    Dwarf_Error*     dw_error);

/*! @brief Given FDE get CIE

    @param dw_fde
    The FDE of interest.
    @param dw_cie_returned
    On success returns a pointer to the applicable CIE.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_cie_of_fde(Dwarf_Fde dw_fde,
    Dwarf_Cie *      dw_cie_returned,
    Dwarf_Error*     dw_error);

/*! @brief Given a CIE get access to its content


    @param dw_cie
    Pass in the CIE of interest.
    @param dw_bytes_in_cie
    On success, returns the length of the CIE in bytes.
    @param dw_version
    On success, returns the CIE version number.
    @param dw_augmenter
    On success, returns a pointer to the augmentation
    string (which could be the empty string).
    @param dw_code_alignment_factor
    On success, returns a the code_alignment_factor
    used to interpret CIE/FDE operations.
    @param dw_data_alignment_factor
    On success, returns a the data_alignment_factor
    used to interpret CIE/FDE operations.
    @param dw_return_address_register_rule
    On success, returns a register number of the
    return address register.
    @param dw_initial_instructions
    On success, returns a pointer to the bytes
    of initial_instructions in the CIE.
    @param dw_initial_instructions_length
    On success, returns the length in bytes of
    the initial_instructions.
    @param dw_offset_size
    On success, returns the offset_size within this CIE.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_cie_info_b(Dwarf_Cie dw_cie,
    Dwarf_Unsigned * dw_bytes_in_cie,
    Dwarf_Small*     dw_version,
    char          ** dw_augmenter,
    Dwarf_Unsigned*  dw_code_alignment_factor,
    Dwarf_Signed*    dw_data_alignment_factor,
    Dwarf_Half*      dw_return_address_register_rule,
    Dwarf_Small   ** dw_initial_instructions,
    Dwarf_Unsigned*  dw_initial_instructions_length,
    Dwarf_Half*      dw_offset_size,
    Dwarf_Error*     dw_error);

/*! @brief Returns CIE index given CIE

    @param dw_cie
    Pass in the CIE of interest.
    @param dw_index
    On success, returns the index (the position
    of the CIE in the CIE pointer array).
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_cie_index(Dwarf_Cie dw_cie,
    Dwarf_Signed* dw_index,
    Dwarf_Error * dw_error);

/*! @brief Returns length and pointer to access frame instructions.

    @see dwarf_expand_frame_instructions

    @param dw_fde
    Pass in the FDE of interest.
    @param dw_outinstrs
    On success returns a pointer to the FDE instruction byte
    stream.
    @param dw_outlen
    On success returns the length of the dw_outinstrs byte
    stream.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
*/
DW_API int dwarf_get_fde_instr_bytes(Dwarf_Fde dw_fde,
    Dwarf_Small   ** dw_outinstrs,
    Dwarf_Unsigned * dw_outlen,
    Dwarf_Error    * dw_error);

/*! @brief Return information on frame registers at a given pc value

    An FDE at a given pc (code address) 

    @param dw_fde
    Pass in the FDE of interest.
    @param dw_pc_requested
    Pass in a pc (code) address inside that FDE. 
    @param dw_reg_table
    On success, returns a pointer to a struct
    given the frame state.
    @param dw_row_pc
    On success returns the address of the row of
    frame data which may be a few counts off of
    the pc requested.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK if the dw_pc_requested is in the
    FDE passed in and there is some applicable row
    in the table.

*/
DW_API int dwarf_get_fde_info_for_all_regs3(Dwarf_Fde dw_fde,
    Dwarf_Addr       dw_pc_requested,
    Dwarf_Regtable3* dw_reg_table,
    Dwarf_Addr*      dw_row_pc,
    Dwarf_Error*     dw_error);

/*  See discussion of dw_value_type, libdwarf.h. */
/*! @brief Returns details about a particular pc and register.

    It is inefficient to iterate across all table_columns (registers)
    using this function.
    Instead call dwarf_get_fde_info_for_all_regs3()
    and index into the table it fills in.

    @param dw_fde
    Pass in the FDE of interest.
    @param dw_table_column
    Pass in the table_column, column numbers in the table
    are 0 through the number_of_registers-1.
    @param dw_pc_requested
    Pass in the pc of interest within dw_fde.
    @param dw_value_type
    On success returns the value type, a DW_EXPR value.
    For example DW_EXPR_EXPRESSION
    @param dw_offset_relevant
    On success returns FALSE if the offset value is
    irrelevant, otherwise TRUE.
    @param dw_register
    On success returns a register number.
    @param dw_offset
    On success returns a register offset value.
    @param dw_block_content
    On success returns a pointer to a block. 
    For example, for DW_EXPR_EXPRESSION the block
    gives access to the expression bytes.
    @param dw_row_pc_out
    On success returns the address of the actual pc
    for this register at this pc.
    @param dw_has_more_rows
    On success returns FALSE if there are no more rows,
    otherwise returns TRUE.
    @param dw_subsequent_pc
    On success this returns the address of the next pc
    for which there is a register row, making access
    to all the rows in sequence much more efficient
    than just adding 1 to a pc value.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK if the dw_pc_requested is in the
    FDE passed in and there is a row for the pc
    in the table.
*/
DW_API int dwarf_get_fde_info_for_reg3_b(Dwarf_Fde dw_fde,
    Dwarf_Half       dw_table_column,
    Dwarf_Addr       dw_pc_requested,
    Dwarf_Small    * dw_value_type,
    Dwarf_Unsigned * dw_offset_relevant,
    Dwarf_Unsigned * dw_register,
    Dwarf_Unsigned * dw_offset,
    Dwarf_Block    * dw_block_content,
    Dwarf_Addr     * dw_row_pc_out,
    Dwarf_Bool     * dw_has_more_rows,
    Dwarf_Addr     * dw_subsequent_pc,
    Dwarf_Error    * dw_error);

/*! @brief Get the value of the CFA for a particular pc value

    @see dwarf_get_fde_info_for_reg3_b  

    This has essentially the same return values but
    it refers to the CFA (which is not part of the register
    table)

*/
DW_API int dwarf_get_fde_info_for_cfa_reg3_b(Dwarf_Fde dw_fde,
    Dwarf_Addr      dw_pc_requested,
    Dwarf_Small   * dw_value_type,
    Dwarf_Unsigned* dw_offset_relevant,
    Dwarf_Unsigned* dw_register,
    Dwarf_Unsigned* dw_offset,
    Dwarf_Block   * dw_block,
    Dwarf_Addr    * dw_row_pc_out,
    Dwarf_Bool    * dw_has_more_rows,
    Dwarf_Addr    * dw_subsequent_pc,
    Dwarf_Error   * dw_error);

/*! @brief Get the fde given DW_AT_MIPS_fde in a DIE.

    This is essentially useless as only SGI compilers
    from the 1990's had  DW_AT_MIPS_fde in the CU DIE.
*/
DW_API int dwarf_get_fde_for_die(Dwarf_Debug dw_dbg,
    Dwarf_Die        dw_subr_die,
    Dwarf_Fde  *     dw_returned_fde,
    Dwarf_Error*     dw_error);

/*! @brief Retrieve an FDE from an FDE table

    This is just like indexing into the FDE array
    but with extra checking of the pointer and
    index. 
    @see dwarf_get_fde_list 
*/
DW_API int dwarf_get_fde_n(Dwarf_Fde* dw_fde_data,
    Dwarf_Unsigned   dw_fde_index,
    Dwarf_Fde      * dw_returned_fde,
    Dwarf_Error    * dw_error);

/*! @brief Retrieve an FDE given a pc

    Using binary search this finds the FDE
    that contains this dw_pc_of_interest
    That works because libdwarf ensures the
    array of FDEs is sorted by the low-pc
    @see dwarf_get_fde_list 

    @param dw_fde_data 
    Pass in a pointer an array of fde pointers.
    @param dw_pc_of_interest 
    The pc value of interest.
    @param dw_returned_fde 
    On success a pointer to the applicable FDE
    is set through the pointer.
    @param dw_lopc 
    On success a pointer to the low pc in dw_returned_fde
    is set through the pointer.
    @param dw_hipc 
    On success a pointer to the high pc (one past
    the actual last byte address) in dw_returned_fde
    is set through the pointer.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK if the dw_pc_of_interest found
    in some FDE in the array.
    If no FDE is found containing dw_pc_of_interest
    DW_DLV_NO_ENTRY is returned.
*/
DW_API int dwarf_get_fde_at_pc(Dwarf_Fde* dw_fde_data,
    Dwarf_Addr   dw_pc_of_interest,
    Dwarf_Fde  * dw_returned_fde,
    Dwarf_Addr * dw_lopc,
    Dwarf_Addr * dw_hipc,
    Dwarf_Error* dw_error);

/*! @brief Return .eh_frame CIE augmentation data.

    GNU .eh_frame CIE augmentation information.
    See Linux Standard Base Core Specification version 3.0 .
    @see https://gcc.gnu.org/legacy-ml/gcc/2003-12/msg01168.html

    @param dw_cie
    The CIE of interest.
    @param dw_augdata
    On success returns a pointer to the augmentation data.
    @param dw_augdata_len
    On success returns the length in bytes of the augmentation data.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
    If the augmentation data length is zero it
    returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_get_cie_augmentation_data(Dwarf_Cie dw_cie,
    Dwarf_Small   ** dw_augdata,
    Dwarf_Unsigned * dw_augdata_len,
    Dwarf_Error*     dw_error);

/*! @brief Return .eh_frame FDE augmentation data.

    GNU .eh_frame FDE augmentation information.
    See Linux Standard Base Core Specification version 3.0 .
    @see https://gcc.gnu.org/legacy-ml/gcc/2003-12/msg01168.html

    @param dw_fde
    The FDE of interest.
    @param dw_augdata
    On success returns a pointer to the augmentation data.
    @param dw_augdata_len
    On success returns the length in bytes of the augmentation data.
    @param dw_error
    The usual error detail return pointer.
    @return
    Returns DW_DLV_OK etc.
    If the augmentation data length is zero it
    returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_get_fde_augmentation_data(Dwarf_Fde dw_fde,
    Dwarf_Small   ** dw_augdata,
    Dwarf_Unsigned * dw_augdata_len,
    Dwarf_Error*     dw_error);

/*! @brief Expands CIE or FDE instructions for detailed examination.
    Called for CIE initial instructions and
    FDE instructions.
    Call dwarf_get_fde_instr_bytes() or
    dwarf_get_cie_info_b() to get the initial instruction bytes
    and instructions byte count you wish to expand.
    @param dw_cie
    The cie relevant to the instructions.
    @param dw_instructionspointer
    points to the instructions
    @param dw_length_in_bytes
    byte length of the instruction sequence.
    @param dw_head
    The address of an allocated dw_head
    @param dw_instr_count
    Returns the numer of inststructions in the byte stream
    @param dw_error
    Error return details
    @return
    On success returns DW_DLV_OK
*/
DW_API int dwarf_expand_frame_instructions(Dwarf_Cie dw_cie,
    Dwarf_Small     * dw_instructionspointer,
    Dwarf_Unsigned    dw_length_in_bytes,
    Dwarf_Frame_Instr_Head * dw_head,
    Dwarf_Unsigned  * dw_instr_count,
    Dwarf_Error     * dw_error);

/*!
    @brief Returns information about a single instruction
    Fields_description means a sequence of up to three
    letters including u,s,r,c,d,b, terminated by NUL byte.
    It is a string but we test individual bytes instead
    of using string compares. Do not free any of the
    returned values.
    @param dw_head
    A head record
    @param dw_instr_index
    index 0 < i < instr_count
    @param dw_instr_offset_in_instrs
    Returns the byte offset of this instruction
    within instructions.
    @param dw_cfa_operation
    Returns a DW_CFA opcode.
    @param dw_fields_description
    Returns a string. Do not free.
    @param dw_u0
    May be set to an unsigned value
    @param dw_u1
    May be set to an unsigned value
    @param dw_s0
    May be set to a signed value
    @param dw_s1
    May be set to a signed value
    @param dw_code_alignment_factor
    May be set by the call
    @param dw_data_alignment_factor
    May be set by the call
    @param dw_expression_block
    Pass in a pointer to a block
    @param dw_error
    If DW_DLV_ERROR and the argument is non-NULL, returns
    details about the error.
    @return On success returns DW_DLV_OK
    If there is no such instruction with that index
    it returns DW_DLV_NO_ENTRY
    On error it returns DW_DLV_ERROR and if dw_error
    is NULL it pushes back a pointer to a Dwarf_Error
    to the caller.

    Frame expressions have a variety of formats
    and content. The dw_fields parameter is set to
    a pointer to
    a short string with some set of the letters
    s,u,r,d,c,b which enables determining exactly
    which values the call sets.
    Some examples:
    A @c s in fields[0] means s0 is a signed number.

    A @c b somewhere in fields means the expression block
    passed in has been filled in.

    A @c r in fields[1] means u1 is set to a register number.

    A @c d in fields means data_alignment_factor is set

    A @c c in fields means code_alignment_factor is set
    There are just nine strings possible and together they
    describe all possible frame instructions.
*/
DW_API int dwarf_get_frame_instruction(
    Dwarf_Frame_Instr_Head  dw_head,
    Dwarf_Unsigned     dw_instr_index,
    Dwarf_Unsigned  *  dw_instr_offset_in_instrs,
    Dwarf_Small     *  dw_cfa_operation,
    const char      ** dw_fields_description,
    Dwarf_Unsigned  *  dw_u0,
    Dwarf_Unsigned  *  dw_u1,
    Dwarf_Signed    *  dw_s0,
    Dwarf_Signed    *  dw_s1,
    Dwarf_Unsigned  *  dw_code_alignment_factor,
    Dwarf_Signed    *  dw_data_alignment_factor,
    Dwarf_Block     *  dw_expression_block,
    Dwarf_Error     *  dw_error);
/*! @brief Expands CIE or FDE instructions for detailed examination.
    Called for CIE initial instructions and
    FDE instructions. This is the same as
    dwarf_get_frame_instruction() except that it adds a
    dw_u2 field which contains an address-space identifier
    if the letter @c a appears in dw_fields_description.
    The dw_u2 field is non-standard and only applies
    to two Heterogenous Debugging
    frame instructions defined by LLVM.

    The return values are the same except here we have:
    an @c a in fields[2] means dw_u2 is an address-space
    identifier for the LLVM CFA instruction.
*/
DW_API int dwarf_get_frame_instruction_a(
    Dwarf_Frame_Instr_Head dw_/* head*/,
    Dwarf_Unsigned     dw_instr_index,
    Dwarf_Unsigned  *  dw_instr_offset_in_instrs,
    Dwarf_Small     *  dw_cfa_operation,
    const char      ** dw_fields_description,
    Dwarf_Unsigned  *  dw_u0,
    Dwarf_Unsigned  *  dw_u1,
    Dwarf_Unsigned  *  dw_u2,
    Dwarf_Signed    *  dw_s0,
    Dwarf_Signed    *  dw_s1,
    Dwarf_Unsigned  *  dw_code_alignment_factor,
    Dwarf_Signed    *  dw_data_alignment_factor,
    Dwarf_Block     *  dw_expression_block,
    Dwarf_Error     *  dw_error);

/*!
    @brief Deallocates the frame instruction data in dw_head
    @param dw_head
    A head pointer.
    Frees all data created by dwarf_expand_frame_instructions()
    and makes the head pointer stale. The caller should
    set  to NULL.
*/
DW_API void dwarf_dealloc_frame_instr_head(Dwarf_Frame_Instr_Head
    dw_head);

/*! @brief  Use to print fde and cie offsets from debugging info.

    @param dw_dbg
    The Dwarf_Debug of interest
    @param dw_in_fde
    Pass in the FDE of interest.
    @param dw_fde_off
    On success returns the section offset of the FDE.
    @param dw_cie_off
    On success returns the section offset of the CIE.
    @param dw_error
    Error return details
    @return
    Returns DW_DLV_OK etc.

*/
DW_API int dwarf_fde_section_offset(Dwarf_Debug dw_dbg,
    Dwarf_Fde     dw_in_fde,
    Dwarf_Off   * dw_fde_off,
    Dwarf_Off   * dw_cie_off,
    Dwarf_Error * dw_error);

/*! @brief  Use to print cie offsets from debugging info.

    @param dw_dbg
    The Dwarf_Debug of interest
    @param dw_in_cie
    Pass in the CIE of interest.
    @param dw_cie_off
    On success returns the section offset of the CIE.
    @param dw_error
    Error return details
    @return
    Returns DW_DLV_OK etc.

*/
DW_API int dwarf_cie_section_offset(Dwarf_Debug dw_dbg,
    Dwarf_Cie     dw_in_cie,
    Dwarf_Off   * dw_cie_off,
    Dwarf_Error * dw_error);


/*! @brief Frame Rule Table Size
    @link frameregs Invariants for setting frame registers @endlink
    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_value
    Pass in the value to record for the library to use.
    @return
    Returns the previous value.
*/
DW_API Dwarf_Half dwarf_set_frame_rule_table_size(
    Dwarf_Debug dw_dbg,
    Dwarf_Half  dw_value);
/*! @brief Frame Rule Initial Value
    
    @link frameregs Invariants for setting frame registers @endlink

    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_value
    Pass in the value to record for the library to use.
    @return
    Returns the previous value.
*/
DW_API Dwarf_Half dwarf_set_frame_rule_initial_value(
    Dwarf_Debug dw_dbg,
    Dwarf_Half  dw_value);
/*! @brief Frame CFA Column 
    @link frameregs Invariants for setting frame registers @endlink
    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_value
    Pass in the value to record for the library to use.
    @return
    Returns the previous value.
*/
DW_API Dwarf_Half dwarf_set_frame_cfa_value(
    Dwarf_Debug dw_dbg,
    Dwarf_Half  dw_value);

/*! @brief Frame Same Value Default
    @link frameregs Invariants for setting frame registers @endlink
    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_value
    Pass in the value to record for the library to use.
    @return
    Returns the previous value.
*/
DW_API Dwarf_Half dwarf_set_frame_same_value(
    Dwarf_Debug dw_dbg,
    Dwarf_Half  dw_value);
/*! @brief Frame Undefined Value Default
    @link frameregs Invariants for setting frame registers @endlink
    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_value
    Pass in the value to record for the library to use.
    @return
    Returns the previous value.
*/
DW_API Dwarf_Half dwarf_set_frame_undefined_value(
    Dwarf_Debug dw_dbg,
    Dwarf_Half  dw_value);
/*! @} */

/*! @defgroup abbrev Abbreviations .debug_abbrev Section Details
    Allows reading .debug_abbrev independently of CUs or DIEs.

    @link  independentsections About Reading Independently. @endlink
    @{
*/
/*! @brief Reading Abbreviation Data

    Normally you never need to call these functions.
    Calls that involve DIEs do all this for you
    behind the scenes in the library.

    This reads the data for a single abbrev code
    starting at dw_offset.
    Essentially, opening access to an abbreviation entry.

    When libdwarf itself reads abbreviations  to
    access DIEs the offset comes
    from the Compilation Unit Header debug_abbrev_offset field.
    @see dwarf_next_cu_header_d

    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_offset
    Pass in the offset where a Debug_Abbrev starts.
    @param dw_returned_abbrev
    On success, sets a pointer to a Dwarf_Abbrev
    through the pointer to allow further access.
    @param dw_length
    On success, returns the length of the entire abbreviation
    block (bytes), useful to calculate the next offset
    if reading the section independently
    of any compilation unit.
    @param dw_attr_count
    On success, returns the number of attributes in this
    abbreviation entry.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
    If the abbreviation is a single zero byte it is
    a null abbreviation. DW_DLV_OK is returned.

    Close the abbrev by calling
    dwarf_dealloc(dbg,*dw_returned_abbrev, DW_DLA_ABBREV)
*/
DW_API int dwarf_get_abbrev(Dwarf_Debug dw_dbg,
    Dwarf_Unsigned  dw_offset,
    Dwarf_Abbrev  * dw_returned_abbrev,
    Dwarf_Unsigned* dw_length,
    Dwarf_Unsigned* dw_attr_count,
    Dwarf_Error*    dw_error);

/*! @brief Get abbreviation tag

    @param dw_abbrev
    The Dwarf_Abbrev of interest.
    @param dw_return_tag_number
    Returns the tag value, for example DW_TAG_compile_unit.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
*/
DW_API int dwarf_get_abbrev_tag(Dwarf_Abbrev dw_abbrev,
    Dwarf_Half*  dw_return_tag_number,
    Dwarf_Error* dw_error);

/*! @brief Get Abbreviation Code

    @param dw_abbrev
    The Dwarf_Abbrev of interest.
    @param dw_return_code_number
    Returns the code for this abbreviation, a number
    assigned to the abbreviation and unique within
    the applicable CU.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
*/
DW_API int dwarf_get_abbrev_code(Dwarf_Abbrev dw_abbrev,
    Dwarf_Unsigned* dw_return_code_number,
    Dwarf_Error*    dw_error);

/*! @brief Get Abbrev Children Flag

    @param dw_abbrev
    The Dwarf_Abbrev of interest.
    @param dw_return_flag
    On success returns the flag TRUE (greater than zero) if the DIE
    referencing the abbreviation has children,
    else returns FALSE (zero).
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
*/
DW_API int dwarf_get_abbrev_children_flag(Dwarf_Abbrev dw_abbrev,
    Dwarf_Signed*    dw_return_flag,
    Dwarf_Error*     dw_error);

/*! @brief Get Abbrev Entry Details

    Most will will call with filter_outliers non-zero.

    @param dw_abbrev
    The Dwarf_Abbrev of interest.
    @param dw_indx
    Valid dw_index values are 0 through dw_attr_count-1
    @param dw_filter_outliers
    Pass non-zero (TRUE) so the function will check for unreasonable
    abbreviation content and return DW_DLV_ERROR if such found.
    If zero (FALSE) passed in
    even a nonsensical attribute number and/or unknown DW_FORM
    are allowed (used by dwarfdump to report the issue(s)).
    @param dw_returned_attr_num
    On success returns the attribute number, such as DW_AT_name
    @param dw_returned_form
    On success returns the attribute FORM, such as DW_FORM_udata
    @param dw_returned_implicit_const
    On success, if the dw_returned_form is DW_FORM_implicit_const
    then dw_returned_implicit_const is the implicit const value,
    but if not implicit const the return value is zero..
    @param dw_offset
    On success returns the offset of the start of this attr/form
    pair in the abbreviation section.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
    If the abbreviation code for this Dwarf_Abbrev is 0
    it is a null abbreviation, the dw_indx is ignored,
    and the function returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_get_abbrev_entry_b(Dwarf_Abbrev dw_abbrev,
    Dwarf_Unsigned   dw_indx,
    Dwarf_Bool       dw_filter_outliers,
    Dwarf_Unsigned * dw_returned_attr_num,
    Dwarf_Unsigned * dw_returned_form,
    Dwarf_Signed   * dw_returned_implicit_const,
    Dwarf_Off      * dw_offset,
    Dwarf_Error    * dw_error);

/*! @} */
/*! @defgroup string String Section .debug_str Details
    Shows just the section content in detail
    @{
*/
/*! @brief Reading From a String Section

    @link independentsections Reading The String Section @endlink 
 
    @param dw_dbg  
    The Dwarf_Debug whose .debug_str section we want to access.
    @param dw_offset  
    Pass in a a string offset. Start at 0, and
    for the next call pass in dw_offset
    plus dw_strlen_of_string plus 1.
    @param dw_string  
    On success returns a pointer to a string from offset
    dw_offset. Never dealloc or free this string.
    @param dw_strlen_of_string
    On success returns the strlen() of the string.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
    If there is no such section or if dw_offset is >=
    the section size it returns DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_get_str(Dwarf_Debug dw_dbg,
    Dwarf_Off        dw_offset,
    char**           dw_string,
    Dwarf_Signed *   dw_strlen_of_string,
    Dwarf_Error*     dw_error);

/*! @} */
/*! @defgroup str_offsets Str_Offsets section details
    Shows just the section content in detail.
    Most library users will never call these,
    as references to this is handled by the code
    accessing some Dwarf_Attribute.
    @link independentsections Reading The Str_Offsets @endlink 
    @{
*/
/*  Allows applications to print the .debug_str_offsets
    section.
    Beginning at starting_offset zero,
    returns data about the first table found.
    The value *next_table_offset is the value
    of the next table (if any), one byte past
    the end of the table whose data is returned..
    Returns DW_DLV_NO_ENTRY if the starting offset
    is past the end of valid data.

    There is no guarantee that there are no non-0 nonsense
    bytes in the section outside of useful tables,
    so this can fail and return nonsense or
    DW_DLV_ERROR  if such garbage exists.
*/

/*! @brief  Allocates a access to a .debug_str_ofrfsets table

    @see examplestrngoffsets

    @param dw_dbg
    Pass in the Dwarf_Debug of interest.
    @param dw_table_data
    On success returns a pointer to an opaque structure
    for use in further calls.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    DW_DLV_OK etc.
    If there is no .debug_str_offsets section it returns
    DW_DLV_NO_ENTRY
*/
DW_API int dwarf_open_str_offsets_table_access(Dwarf_Debug dw_dbg,
    Dwarf_Str_Offsets_Table * dw_table_data,
    Dwarf_Error             * dw_error);

/*! @brief  Close str_offsets access, free table_data.

    @see examplestrngoffsets
 
    @param dw_table_data
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    DW_DLV_OK etc.
    If there is no .debug_str_offsets section it returns
    DW_DLV_NO_ENTRY
    If it returns DW_DLV_ERROR there is nothing you can do
    except reportthe error and, optionally,
    call dwarf_dealloc_error to dealloc the error content
    (and then set the dw_error to NULL as after the dealloc
    the pointer is stale).. 
*/
DW_API int dwarf_close_str_offsets_table_access(
    Dwarf_Str_Offsets_Table  dw_table_data,
    Dwarf_Error            * dw_error);

/*! @brief Iterate through the offsets tables

    @see examplestrngoffsets
 
    Access to the tables starts at offset zero.
    The library progresses through the next table
    automatically, keeping track internally to 
    know where it is.

    @param dw_table_data
    Pass in an open Dwarf_Str_Offsets_Table.
    @param dw_unit_length
    On success returns a table unit_length field
    @param dw_unit_length_offset
    On success returns the section offset of the unit_length field.
    @param dw_table_start_offset
    On success returns the section offset of the array
    of table entries.
    @param dw_entry_size
    On success returns the entry size (4 or 8)
    @param dw_version
    On success returns the value in the version field 5.
    @param dw_padding
    On success returns the zero value in the padding field.
    @param dw_table_value_count
    On success returns the number of table entries, each
    of size dw_entry_size, in the table.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    DW_DLV_OK
    Returns DW_DLV_NO_ENTRY if there are no more entries.
*/
DW_API int dwarf_next_str_offsets_table(
    Dwarf_Str_Offsets_Table dw_table_data,
    Dwarf_Unsigned * dw_unit_length,
    Dwarf_Unsigned * dw_unit_length_offset,
    Dwarf_Unsigned * dw_table_start_offset,
    Dwarf_Half     * dw_entry_size,
    Dwarf_Half     * dw_version,
    Dwarf_Half     * dw_padding,
    Dwarf_Unsigned * dw_table_value_count,
    Dwarf_Error    * dw_error);

/*! @brief Access to an individual str offsets table entry

    @see examplestrngoffsets
 
    @param dw_table_data 
    Pass in the open table pointer.
    @param dw_index_to_entry
    Pass in the entry number, 0 through dw_table_value_count-1
    for the active table
    @param dw_entry_value
    On success returns the value in that table entry,
    an offset into a string table.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    DW_DLV_OK
    Returns DW_DLV_ERROR if dw_index_to_entry is
    out of the correct range.
*/
DW_API int dwarf_str_offsets_value_by_index(
    Dwarf_Str_Offsets_Table dw_table_data,
    Dwarf_Unsigned   dw_index_to_entry,
    Dwarf_Unsigned * dw_entry_value,
    Dwarf_Error    * dw_error);

/*! @brief Reports final wasted-bytes count
    
    Reports the number of tables seen so far.
    Not very interesting.

    @param dw_table_data 
    Pass in the open table pointer.
    @param dw_wasted_byte_count
    Always returns 0 at present.
    @param dw_table_count 
    On success returns the total number of tables seen
    so far in the section.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    DW_DLV_OK etc.
*/
DW_API int dwarf_str_offsets_statistics(
    Dwarf_Str_Offsets_Table dw_table_data,
    Dwarf_Unsigned * dw_wasted_byte_count,
    Dwarf_Unsigned * dw_table_count,
    Dwarf_Error    * dw_error);

/*! @} */
/*! @defgroup dwarferror Dwarf_Error Functions
    These functions aid in understanding handling.
    @{
*/
/*! @brief What DW_DLE code does the error have?
    @param dw_error
    The dw_error should be non-null and a valid Dwarf_Error.
    @return
    A DW_DLE value of some kind. For example: DW_DLE_DIE_NULL.
*/
DW_API Dwarf_Unsigned dwarf_errno(Dwarf_Error dw_error);
/*! @brief What message string is in the error?
    @param dw_error
    The dw_error should be non-null and a valid Dwarf_Error.
    @return
    A string with a message related to the error.
*/
DW_API char* dwarf_errmsg(Dwarf_Error dw_error);
/*! @brief What message string is associated with the error number.
    @param dw_errornum
    The dw_error should be an integer from the DW_DLE set.
    For example, DW_DLE_DIE_NULL.
    @return
    The generic string describing that error number.
*/
DW_API char* dwarf_errmsg_by_number(Dwarf_Unsigned dw_errornum);

/*! @brief Creating an error.
    This is very rarely helpful.  It lets the library
    user create a Dwarf_Error and associate any string
    with that error. Your code could then return
    DW_DLV_ERROR to your caller when your intent
    is to let your caller clean up whatever seems wrong.
    @param dw_dbg
    The relevant Dwarf_Debugr.
    @param dw_error
    a Dwarf_Error is returned through this pointer.
    @param dw_errmsg
    The message string you provide.
*/
DW_API void  dwarf_error_creation(Dwarf_Debug dw_dbg ,
    Dwarf_Error * dw_error, char * dw_errmsg);

/*! @brief Free (dealloc) an Dwarf_Error somethimg created.
    @param dw_dbg
    The relevant Dwarf_Debug pointer.
    @param dw_error
    A pointer to a Dwarf_Error.  The pointer is then stale
    so you should immediately zero that pointer passed
    in.
*/
DW_API void dwarf_dealloc_error(Dwarf_Debug dw_dbg,
    Dwarf_Error dw_error);
/*! @} */

/*! @defgroup dwarfdealloc Generic dwarf_dealloc Function
    Works for most dealloc needed.

    For easier to use versions see the following
    @see dwarf_dealloc_attribute @see dwarf_dealloc_die
    @see dwarf_dealloc_dnames @see dwarf_dealloc_error
    @see dwarf_dealloc_fde_cie_list
    @see dwarf_dealloc_frame_instr_head
    @see dwarf_dealloc_macro_context @see dwarf_dealloc_ranges
    @see dwarf_dealloc_rnglists_head
    @see dwarf_dealloc_uncompressed_block
    @see dwarf_funcs_dealloc @see dwarf_globals_dealloc
    @see dwarf_gnu_index_dealloc @see dwarf_loc_head_c_dealloc
    @see dwarf_pubtypes_dealloc @see dwarf_srclines_dealloc_b
    @see dwarf_types_dealloc @see dwarf_vars_dealloc
    @see dwarf_weaks_dealloc

    @{
*/
/*! @brief The generic dealloc (free) function.
    It requires you know the correct DW_DLA value
    to pass in, and in a few cases such is
    not provided. The functions doing allocations
    tell you which dealloc to use.

    @param dw_dbg
    Must be a valid open Dwarf_Debug.
    and must be the dw_dbg that the error
    was created on.
    If it is not the dealloc will do nothing.
    @param dw_space
    Must be an address returned directly
    by a libdwarf call that the
    call specifies as requiring
    dealloc/free.  If it is not
    a segfault or address fault is possible.
    @param dw_type
    Must be a correct naming of the DW_DLA type.
    If it is not the dealloc will do nothing.
*/
DW_API void dwarf_dealloc(Dwarf_Debug dw_dbg,
    void* dw_space, Dwarf_Unsigned dw_type);
/*! @} */
/*! @defgroup debugsup Access to Section .debug_sup
    @{
*/
/*! @brief Returns basic .debug_sup section header data

    This returns basic data from the header
    of a .debug_sup section.
    See DWARF5 Section 7.3.6,
    "DWARF Supplementary Object Files"

    Other sections present should be normal
    DWARF5, so normal libdwarf calls should work.
    We have no existing examples on hand, so
    it is hard to know what really works.

    If there is no such section it returns
    DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_get_debug_sup(Dwarf_Debug dw_dbg,
    Dwarf_Half     * dw_version,
    Dwarf_Small    * dw_is_supplementary,
    char          ** dw_filename,
    Dwarf_Unsigned * dw_checksum_len,
    Dwarf_Small   ** dw_checksum,
    Dwarf_Error    * dw_error);
/*! @} */

/*! @defgroup debugnames Fast Access-Access to .debug_names DWARF5
    @{

    The section is new in DWARF5  supercedes .debug_pubnames and
    .debug_pubtypes in DWARF2, DWARF3, and DWARF4.

    The code is incomplete . We have no examples
    produced by a compiler as yet.

    The existing functions provide a detailed reporting
    of the content and structure of the table, they
    are not intended to be used to search the table.

    A new function (more than one?) would be needed for convenient
    searching.
*/
/*! @brief Open access to a .debug_names table
    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_starting_offset
    Read this section starting at offset zero.
    @param dw_dn
    On success returns a pointer to a set of data
    allowing access to the table.
    @param dw_offset_of_next_table
    On success returns
    Offset just past the end of the the opened table.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
    If there is no such table or if dw_starting_offset
    is past the end of the section it returns
    DW_DLV_NO_ENTRY.
*/
DW_API int dwarf_dnames_header(Dwarf_Debug dw_dbg,
    Dwarf_Off           dw_starting_offset,
    Dwarf_Dnames_Head * dw_dn,
    Dwarf_Off         * dw_offset_of_next_table,
    Dwarf_Error *       dw_error);

/*! @brief  Frees all the malloc data associated with dw_dn
    @param dw_dn
    A Dwarf_Dnames_Head pointer.
    Callers should zero the pointer passed in
    as soon as possible after this returns
    as the pointer is then stale.
*/
DW_API void dwarf_dealloc_dnames(Dwarf_Dnames_Head dw_dn);

/*! @brief Sizes and counts from the debug names table

    We do not describe these returned values.

    See DWARF5 section 6.1.1 "Lookup By Name"
    particularly the graph page 139.
    dw_comp_unit_count is K(k),
    dw_local_type_unit_count is T(t), and
    dw_foreign_type_unit_count is F(f).
*/
DW_API int dwarf_dnames_sizes(Dwarf_Dnames_Head dw_dn,
    Dwarf_Unsigned * dw_comp_unit_count,
    Dwarf_Unsigned * dw_local_type_unit_count,
    Dwarf_Unsigned * dw_foreign_type_unit_count,
    Dwarf_Unsigned * dw_bucket_count,
    Dwarf_Unsigned * dw_name_count,
    /* The following are counted in bytes */
    Dwarf_Unsigned * dw_abbrev_table_size,
    Dwarf_Unsigned * dw_entry_pool_size,
    Dwarf_Unsigned * dw_augmentation_string_size,
    char          ** dw_augmentation_string,
    Dwarf_Unsigned * dw_section_size,
    Dwarf_Half     * dw_table_version,
    Dwarf_Half     * dw_offset_size,
    Dwarf_Error *    dw_error);

/*! @brief each debug names list entry one at a time
    @param dw_dn
    The table of interest.
    @param dw_type
    Pass in the type, "cu" or "tu"
    @param dw_index_number
    For "cu" index range is 0 through K-1
    For "tu" index range is 0 through T+F-1
    @param dw_offset
    Section offset of the target CU
    Zero if it cannot be determined.
    @param dw_sig
    the Dwarf_Sig8 is filled in wht a signature
    if the TU index is T through T+F-1
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
*/
DW_API int dwarf_dnames_cu_table(Dwarf_Dnames_Head dw_dn,
    const char        * dw_type,
    Dwarf_Unsigned      dw_index_number,
    Dwarf_Unsigned    * dw_offset,
    Dwarf_Sig8        * dw_sig,
    Dwarf_Error       * dw_error);

/*! @brief access to bucket contents.
    @param dw_dn
    The Dwarf_Dnames_Head of interest.
    @param dw_bucket_number
    Pass in a bucket number
    Bucket numbers start at 0.
    @param dw_index
    On success returns the index of
    the appropriate name entry.
    @param dw_indexcount
    On success returns the number of
    name entries in the bucket.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
    An out of range dw_index_number gets a return
    if DW_DLV_NO_ENTRY
*/
DW_API int dwarf_dnames_bucket(Dwarf_Dnames_Head dw_dn,
    Dwarf_Unsigned      dw_bucket_number,
    Dwarf_Unsigned    * dw_index,
    Dwarf_Unsigned    * dw_indexcount,
    Dwarf_Error *       dw_error);

/*! @brief retrieve a name table entry
    @param dw_dn
    The table of interest.
    @param dw_name_index
    Pass in the desired index, start at zero.
    @param dw_bucket_number
    On success returns a bucket number.
    @param dw_hash_value
    The hash value
    @param dw_offset_to_debug_str
    The offset to the .debug_str section string.
    @param dw_ptrtostr
    if dw_ptrtostr non-null returns a pointer to
    the applicable string here.
    @param dw_offset_in_entrypool
    Returns the offset in the entrypool
    @param dw_abbrev_number
    Returned from entrypool
    @param dw_abbrev_tag
    Returned from entrypool abbrev data
    @param dw_array_size
    Size of array you provide (even number).
    Possibly 20 to 40 suffices for pracical purposes.
    @param dw_attr_array
    Array you provide, for attribute numbers, form numbers.
    (function will initialize it).
    As initialized the last pair will be 0,0
    @param dw_attr_count
    Array entries needed. Might be larger than
    dw_array_size, meaning not all entries could
    be returned in your array.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
*/
DW_API int dwarf_dnames_name(Dwarf_Dnames_Head dw_dn,
    Dwarf_Unsigned      dw_name_index,
    Dwarf_Unsigned    * dw_bucket_number,
    Dwarf_Unsigned    * dw_hash_value,
    Dwarf_Unsigned    * dw_offset_to_debug_str,
    char *            * dw_ptrtostr,
    Dwarf_Unsigned    * dw_offset_in_entrypool,
    Dwarf_Unsigned    * dw_abbrev_number,
    Dwarf_Half        * dw_abbrev_tag,
    Dwarf_Unsigned      dw_array_size,
    Dwarf_Half        * dw_attr_array,
    Dwarf_Unsigned    * dw_attr_count,
    Dwarf_Error *       dw_error);
/*! @} */

/*! @defgroup aranges Fast Access-Access to a CU given a code address
    @{
*/
/*! @brief Get access to CUs given code addresses

    This intended as a fast-access to tie code addresses
    to CU dies. The data is in the .debug_aranges section.
    which may appear in DWARF2,3,4, or DWARF5.

    @see exampleu
    
    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_aranges
    On success returns a pointer to an array
    of Dwarf_Arange pointers.
    @param dw_arange_count
    On success returns a count of the length of the array.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
    Returns DW_DLV_NO_ENTRY if there is no such section.
*/
DW_API int dwarf_get_aranges(Dwarf_Debug dw_dbg,
    Dwarf_Arange**   dw_aranges,
    Dwarf_Signed *   dw_arange_count,
    Dwarf_Error*     dw_error);

/*! @brief Find a range given a code address

    @param dw_aranges
    Pass in a pointer to the first entry in the aranges array
    of pointers.
    @param dw_arange_count
    Pass in the dw_arange_count, the count for the array.
    @param dw_address
    Pass in the code address of interest.
    @param dw_returned_arange
    On success, returns the particular arange that
    holds that address.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
    Returns DW_DLV_NO_ENTRY if there is no such code
    address present in the section.
*/
DW_API int dwarf_get_arange(Dwarf_Arange* dw_aranges,
    Dwarf_Unsigned   dw_arange_count,
    Dwarf_Addr       dw_address,
    Dwarf_Arange *   dw_returned_arange,
    Dwarf_Error*     dw_error);

/*! @brief Given an arange return its CU DIE offset.

    @param dw_arange 
    The specific arange of interest.
    @param dw_return_offset 
    The CU DIE offset (in .debug_info) applicable
    to this arange..
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
*/
DW_API int dwarf_get_cu_die_offset(Dwarf_Arange dw_arange,
    Dwarf_Off  * dw_return_offset,
    Dwarf_Error* dw_error);

/*! @brief Given an arange return its CU header offset.

    @param dw_arange
    The specific arange of interest.
    @param dw_return_cu_header_offset
    The CU header offset (in .debug_info) applicable
    to this arange.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
*/
DW_API int dwarf_get_arange_cu_header_offset(Dwarf_Arange dw_arange,
    Dwarf_Off  * dw_return_cu_header_offset,
    Dwarf_Error* dw_error);

/*! @brief Get the data in an arange entry. 

    @param dw_arange
    The specific arange of interest.
    @param dw_segment
    On success and if segment_entry_size is non-zero
    this returns the segment number
    from the arange.
    @param dw_segment_entry_size
    On success returns the segment entry size
    from the arange.
    @param dw_start
    On success returns the low address this arange
    refers to.
    @param dw_length
    On success returns the length, in bytes of the
    code area this arange refers to.
    @param dw_cu_die_offset
    On success returns the .debug_info section offset
    the arange refers to.
    @param dw_error
    On error dw_error is set to point to the error details.
    @return
    The usual value: DW_DLV_OK etc.
*/
DW_API int dwarf_get_arange_info_b(Dwarf_Arange dw_arange,
    Dwarf_Unsigned*  dw_segment,
    Dwarf_Unsigned*  dw_segment_entry_size,
    Dwarf_Addr    *  dw_start,
    Dwarf_Unsigned*  dw_length,
    Dwarf_Off     *  dw_cu_die_offset,
    Dwarf_Error   *  dw_error );
/*! @} */

/*! @defgroup pubnames Fast Access-Access to .debug_pubnames and more.
    @{ */

/*  global name space operations (.debug_pubnames access)
    The pubnames and similar sections are rarely used. Few compilers
    emit them. They are DWARF 2,3,4 only., not DWARF 5.  */

/*  New March 2019. Special for dwarfdump.
    Sets a flag in the dbg. Always returns DW_DLV_OK
    and never touches error */
DW_API int dwarf_return_empty_pubnames(Dwarf_Debug /*dbg*/,
    int /* flag */,
    Dwarf_Error* /*error*/);

DW_API int dwarf_get_globals(Dwarf_Debug /*dbg*/,
    Dwarf_Global**   /*globals*/,
    Dwarf_Signed *   /*number_of_globals*/,
    Dwarf_Error*     /*error*/);

DW_API void dwarf_globals_dealloc(Dwarf_Debug /*dbg*/,
    Dwarf_Global*    /*globals*/,
    Dwarf_Signed     /*number_of_globals*/);

/*  The following four are closely related. */
DW_API int dwarf_globname(Dwarf_Global /*glob*/,
    char   **        /*returned_name*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_global_die_offset(Dwarf_Global /*global*/,
    Dwarf_Off*       /*return_offset*/,
    Dwarf_Error *    /*error*/);
DW_API int dwarf_global_cu_offset(Dwarf_Global /*global*/,
    Dwarf_Off*       /*return_offset, offset of cu header*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_global_name_offsets(Dwarf_Global /*global*/,
    char   **        /*returned_name*/,
    Dwarf_Off*       /*die_offset*/,
    Dwarf_Off*       /*cu_offset, offset of
        cu die*/,
    Dwarf_Error*     /*error*/);

/*  New February 2019. For more complete dwarfdump printing.
    For each CU represented in .debug_pubnames, etc,
    there is a .debug_pubnames header.  For any given
    Dwarf_Global this returns the content of the applicable
    header. */
DW_API int dwarf_get_globals_header(Dwarf_Global /*global*/,
    Dwarf_Off      * /*offset_pub_header*/,
    Dwarf_Unsigned * /*length_size*/,
    Dwarf_Unsigned * /*length_pub*/,
    Dwarf_Unsigned * /*version*/,
    Dwarf_Unsigned * /*header_info_offset*/,
    Dwarf_Unsigned * /*info_length*/,
    Dwarf_Error*   /*error*/);

/* Static function name operations.  */
DW_API int dwarf_get_funcs(Dwarf_Debug    /*dbg*/,
    Dwarf_Func**     /*funcs*/,
    Dwarf_Signed *   /*number_of_funcs*/,
    Dwarf_Error*     /*error*/);
DW_API void dwarf_funcs_dealloc(Dwarf_Debug /*dbg*/,
    Dwarf_Func*      /*funcs*/,
    Dwarf_Signed     /*number_of_funcs*/);

/* The following four are closely related */
DW_API int dwarf_funcname(Dwarf_Func /*func*/,
    char   **        /*returned_name*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_func_die_offset(Dwarf_Func /*func*/,
    Dwarf_Off*       /*return_offset*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_func_cu_offset(Dwarf_Func /*func*/,
    Dwarf_Off*       /*return_offset of the
        cu header*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_func_name_offsets(Dwarf_Func /*func*/,
    char   **        /*returned_name*/,
    Dwarf_Off*       /*die_offset*/,
    Dwarf_Off*       /*cu_offset of the
        cu die*/,
    Dwarf_Error*     /*error*/);

/*  jUser-defined type name operations,
    SGI IRIX .debug_typenames section.

    Same content as DWARF3 .debug_pubtypes, but
    defined years before .debug_pubtypes was defined.
    SGI IRIX only. */
DW_API int dwarf_get_types(Dwarf_Debug    /*dbg*/,
    Dwarf_Type**     /*types*/,
    Dwarf_Signed *   /*number_of_types*/,
    Dwarf_Error*     /*error*/);
DW_API void dwarf_types_dealloc(Dwarf_Debug /*dbg*/,
    Dwarf_Type*      /*types*/,
    Dwarf_Signed     /*number_of_types*/);

/*  The fourth gives all the values that the next
    three combined do. */
DW_API int dwarf_typename(Dwarf_Type /*type*/,
    char   **        /*returned_name*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_type_die_offset(Dwarf_Type /*type*/,
    Dwarf_Off*       /*return_offset*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_type_cu_offset(Dwarf_Type /*type*/,
    Dwarf_Off*       /*return_offset is offset of
        cu_header */,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_type_name_offsets(Dwarf_Type    /*type*/,
    char   **        /*returned_name*/,
    Dwarf_Off*       /*die_offset*/,
    Dwarf_Off*       /*cu_offset is offset of
        cu_die */,
    Dwarf_Error*     /*error*/);

/*  User-defined type name operations, DWARF3  .debug_pubtypes
    section.  */
DW_API int dwarf_get_pubtypes(Dwarf_Debug    /*dbg*/,
    Dwarf_Type**     /*types*/,
    Dwarf_Signed *   /*number_of_types*/,
    Dwarf_Error*     /*error*/);
DW_API void dwarf_pubtypes_dealloc(Dwarf_Debug /*dbg*/,
    Dwarf_Type*      /*pubtypes*/,
    Dwarf_Signed     /*number_of_pubtypes*/);

/*  The first three present the same information
    as the fourth here does in one call.
    Probably best to use the fourth one and ignore the
    first three. cu_offsset is cu_header offset. */
DW_API int dwarf_pubtypename(Dwarf_Type /*type*/,
    char   **        /*returned_name*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_pubtype_type_die_offset(Dwarf_Type /*type*/,
    Dwarf_Off*       /*return_offset*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_pubtype_cu_offset(Dwarf_Type /*type*/,
    Dwarf_Off*       /*return_offset is offset
        of cu_header */,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_pubtype_name_offsets(Dwarf_Type    /*type*/,
    char   **        /*returned_name*/,
    Dwarf_Off*       /*die_offset*/,
    Dwarf_Off*       /*cu_offset is offset of
        cu_die */,
    Dwarf_Error*     /*error*/);

/* File-scope static variable name operations.  */
DW_API int dwarf_get_vars(Dwarf_Debug    /*dbg*/,
    Dwarf_Var**      /*vars*/,
    Dwarf_Signed *   /*number_of_vars*/,
    Dwarf_Error*     /*error*/);
DW_API void dwarf_vars_dealloc(Dwarf_Debug /*dbg*/,
    Dwarf_Var*       /*vars*/,
    Dwarf_Signed     /*number_of_vars*/);

/*  The following four closely related. */
DW_API int dwarf_varname(Dwarf_Var /*var*/,
    char   **        /*returned_name*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_var_die_offset(Dwarf_Var /*var*/,
    Dwarf_Off*       /*return_offset*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_var_cu_offset(Dwarf_Var /*var*/,
    Dwarf_Off*       /*return_offset of
        the cu header*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_var_name_offsets(Dwarf_Var /*var*/,
    char   **        /*returned_name*/,
    Dwarf_Off*       /*die_offset*/,
    Dwarf_Off*       /*cu_offset of the
        cu die */,
    Dwarf_Error*     /*error*/);

/* weak name operations.  */
DW_API int dwarf_get_weaks(Dwarf_Debug    /*dbg*/,
    Dwarf_Weak**     /*weaks*/,
    Dwarf_Signed *   /*number_of_weaks*/,
    Dwarf_Error*     /*error*/);
DW_API void dwarf_weaks_dealloc(Dwarf_Debug /*dbg*/,
    Dwarf_Weak*      /*weaks*/,
    Dwarf_Signed     /*number_of_weaks*/);

/*  The following four closely related. */
DW_API int dwarf_weakname(Dwarf_Weak /*weak*/,
    char   **        /*returned_name*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_weak_die_offset(Dwarf_Weak /*weak*/,
    Dwarf_Off*       /*return_offset*/,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_weak_cu_offset(Dwarf_Weak /*weak*/,
    Dwarf_Off*       /*return_offset of
        the CU header */,
    Dwarf_Error*     /*error*/);
DW_API int dwarf_weak_name_offsets(Dwarf_Weak    /*weak*/,
    char   **        /*returned_name*/,
    Dwarf_Off*       /*die_offset*/,
    Dwarf_Off*       /*cu_offset of
        the CU die */,
    Dwarf_Error*     /*error*/);
/*! @} */

/*! @defgroup gnupubnames Fast Access-Access GNU .debug_gnu_pubnames
    @{
*/
/*  BEGIN: .debug_gnu_pubnames .debug_gnu_typenames access,
    calling these  Gnu_Index as a general reference.  */
DW_API int dwarf_get_gnu_index_head(Dwarf_Debug /*dbg*/,
    /*  The following arg false to select gnu_pubtypes */
    Dwarf_Bool             /*for_gdb_pubnames*/ ,
    Dwarf_Gnu_Index_Head * /*index_head_out*/,
    Dwarf_Unsigned       * /*index_block_count_out*/,
    Dwarf_Error * /*error*/);
/*  Frees all resources used for the indexes. */
DW_API void dwarf_gnu_index_dealloc(Dwarf_Gnu_Index_Head /*head*/);

DW_API int dwarf_get_gnu_index_block(Dwarf_Gnu_Index_Head /*head*/,
    Dwarf_Unsigned     /*number*/,
    Dwarf_Unsigned   * /*block_length */,
    Dwarf_Half       * /*version */,
    Dwarf_Unsigned   * /*offset_into_debug_info*/,
    Dwarf_Unsigned   * /*size_of_debug_info_area*/,
    Dwarf_Unsigned   * /*count_of_index_entries*/,
    Dwarf_Error      * /*error*/);

DW_API int dwarf_get_gnu_index_block_entry(
    Dwarf_Gnu_Index_Head /*head*/,
    Dwarf_Unsigned    /*blocknumber*/,
    Dwarf_Unsigned    /*entrynumber*/,
    Dwarf_Unsigned  * /*offset_in_debug_info*/,
    const char     ** /*name_string*/,
    unsigned char   * /*flagbyte*/,
    unsigned char   * /*staticorglobal*/,
    unsigned char   * /*typeofentry*/,
    Dwarf_Error     * /*error*/);
/* END: debug_gnu_pubnames/typenames access, */

/*! @} */

/*! @defgroup gdbindex Fast Access-Gdb Index into .dwo or .dwp
    If .gdb_index present in an executable then
    the content here helps quickly find their way
    into the .dwo or .dwp .debug_cu_index or
    .debug_tu_index split dwarf sections.

    This is a section created for and used by the GNU gdb
    debugger.  Not part of standard DWARF.
    @{
*/

/*  .gdb_index section operations.
    A GDB extension. Completely different than
    .debug_gnu_pubnames or .debug_gnu_pubtypes sections.
    The section is in some executables and if present
    is used to quickly map an address or name to
    a skeleton CU or TU.  If present then there are
    .dwo or .dwp files somewhere to make detailed
    debugging possible (up to user code to
    find it/them and deal with them).

    Version 8 built by gdb, so type entries are ok as is.
    Version 7 built by the 'gold' linker and type index
    entries for a CU must be derived othewise, the
    type index is not correct... ? FIXME
    */

/*  Creates a Dwarf_Gdbindex, returning it and
    its values through the pointers. */
DW_API int dwarf_gdbindex_header(Dwarf_Debug /*dbg*/,
    Dwarf_Gdbindex * /*gdbindexptr*/,
    Dwarf_Unsigned * /*version*/,
    Dwarf_Unsigned * /*cu_list_offset*/,
    Dwarf_Unsigned * /*types_cu_list_offset*/,
    Dwarf_Unsigned * /*address_area_offset*/,
    Dwarf_Unsigned * /*symbol_table_offset*/,
    Dwarf_Unsigned * /*constant_pool_offset*/,
    Dwarf_Unsigned * /*section_size*/,
    const char    ** /*section_name*/,
    Dwarf_Error    * /*error*/);

DW_API int dwarf_gdbindex_culist_array(Dwarf_Gdbindex /*gdbindexptr*/,
    Dwarf_Unsigned       * /*list_length*/,
    Dwarf_Error          * /*error*/);

/*  entryindex: 0 to list_length-1 */
DW_API int dwarf_gdbindex_culist_entry(Dwarf_Gdbindex /*gdbindexptr*/,
    Dwarf_Unsigned   /*entryindex*/,
    Dwarf_Unsigned * /*cu_offset*/,
    Dwarf_Unsigned * /*cu_length*/,
    Dwarf_Error    * /*error*/);

DW_API int dwarf_gdbindex_types_culist_array(Dwarf_Gdbindex
    /*ndexptr*/,
    Dwarf_Unsigned            * /*types_list_length*/,
    Dwarf_Error               * /*error*/);

/*  entryindex: 0 to types_list_length -1 */
DW_API int dwarf_gdbindex_types_culist_entry(
    Dwarf_Gdbindex   /*gdbindexptr*/,
    Dwarf_Unsigned   /*entryindex*/,
    Dwarf_Unsigned * /*cu_offset*/,
    Dwarf_Unsigned * /*tu_offset*/,
    Dwarf_Unsigned * /*type_signature*/,
    Dwarf_Error    * /*error*/);

DW_API int dwarf_gdbindex_addressarea(Dwarf_Gdbindex /*gdbindexptr*/,
    Dwarf_Unsigned            * /*addressarea_list_length*/,
    Dwarf_Error               * /*error*/);

/*    entryindex: 0 to addressarea_list_length-1 */
DW_API int dwarf_gdbindex_addressarea_entry(Dwarf_Gdbindex
    /*gdbindexptr*/,
    Dwarf_Unsigned   /*entryindex*/,
    Dwarf_Unsigned * /*low_adddress*/,
    Dwarf_Unsigned * /*high_address*/,
    Dwarf_Unsigned * /*cu_index*/,
    Dwarf_Error    * /*error*/);

DW_API int dwarf_gdbindex_symboltable_array(Dwarf_Gdbindex
    /*gdbindexptr*/,
    Dwarf_Unsigned            * /*symtab_list_length*/,
    Dwarf_Error               * /*error*/);

/*  entryindex: 0 to symtab_list_length-1 */
DW_API int dwarf_gdbindex_symboltable_entry(Dwarf_Gdbindex
    /*gdbindexptr*/,
    Dwarf_Unsigned   /*entryindex*/,
    Dwarf_Unsigned * /*string_offset*/,
    Dwarf_Unsigned * /*cu_vector_offset*/,
    Dwarf_Error    * /*error*/);

DW_API int dwarf_gdbindex_cuvector_length(Dwarf_Gdbindex /*gdbindex*/,
    Dwarf_Unsigned   /*cuvector_offset*/,
    Dwarf_Unsigned * /*innercount*/,
    Dwarf_Error    * /*error*/);

DW_API int dwarf_gdbindex_cuvector_inner_attributes(
    Dwarf_Gdbindex /*index*/,
    Dwarf_Unsigned   /*cuvector_offset*/,
    Dwarf_Unsigned   /*innerindex*/,
    /* The attr_value is a field of bits. For expanded version
        use  dwarf_gdbindex_cuvector_instance_expand_value() */
    Dwarf_Unsigned * /*attr_value*/,
    Dwarf_Error    * /*error*/);

DW_API int dwarf_gdbindex_cuvector_instance_expand_value(
    Dwarf_Gdbindex,
    Dwarf_Unsigned   /*value*/,
    Dwarf_Unsigned * /*cu_index*/,
    Dwarf_Unsigned * /*symbol_kind*/,
    Dwarf_Unsigned * /*is_static*/,
    Dwarf_Error    * /*error*/);

/*  The strings in the pool follow (in memory) the cu index
    set and are NUL terminated. */

DW_API int dwarf_gdbindex_string_by_offset(Dwarf_Gdbindex
    /*gdbindexptr*/,
    Dwarf_Unsigned   /*stringoffset*/,
    const char    ** /*string_ptr*/,
    Dwarf_Error   *  /*error*/);

DW_API void dwarf_gdbindex_free(Dwarf_Gdbindex /*gdbindexptr*/);

/*! @} */

/*! @defgroup splitdwarf Fast Access-Split Dwarf (Debug Fission)
    @{
*/
/*  START debugfission dwp .debug_cu_index
    and .debug_tu_index operations. */

DW_API int dwarf_get_xu_index_header(Dwarf_Debug /*dbg*/,
    const char *  section_type, /* "tu" or "cu" */
    Dwarf_Xu_Index_Header *     /*xuhdr*/,
    Dwarf_Unsigned *            /*version_number*/,
    Dwarf_Unsigned *            /*offsets_count L*/,
    Dwarf_Unsigned *            /*units_count N*/,
    Dwarf_Unsigned *            /*hash_slots_count M*/,
    const char     **           /*sect_name*/,
    Dwarf_Error *               /*err*/);

DW_API int dwarf_get_xu_index_section_type(Dwarf_Xu_Index_Header
    /*xuhdr*/,
    /*  the function returns a pointer to
        the immutable string "tu" or "cu" via this arg.
        Do not free.  */
    const char ** /*typename*/,
    /*  the function returns a pointer to
        the immutable section name. Do not free.
        .debug_cu_index or .debug_tu_index */
    const char ** /*sectionname*/,
    Dwarf_Error * /*err*/);

/*  Index values 0 to M-1 are valid. */
DW_API int dwarf_get_xu_hash_entry(Dwarf_Xu_Index_Header /*xuhdr*/,
    Dwarf_Unsigned     /*index*/,

    /*  Returns the hash value. 64  bits.  */
    Dwarf_Sig8 *      /*hash_value*/,

    /* returns the index into rows of offset/size tables. */
    Dwarf_Unsigned *  /*index_to_sections*/,
    Dwarf_Error *     /*err*/);

/*  Columns 0 to L-1,  valid. */
DW_API int dwarf_get_xu_section_names(Dwarf_Xu_Index_Header /*xuhdr*/,
    /* Row index defined to be row zero. */
    Dwarf_Unsigned  /*column_index*/,
    Dwarf_Unsigned* /*DW_SECT_ number*/,
    const char **   /*DW_SECT_ name*/,
    Dwarf_Error *   /*err*/);

    /* Rows 1 to N col 0 to L-1  are valid */
DW_API int dwarf_get_xu_section_offset(Dwarf_Xu_Index_Header /*xuhd*/,
    Dwarf_Unsigned  /*row_index*/,
    Dwarf_Unsigned  /*column_index*/,
    Dwarf_Unsigned* /*sec_offset*/,
    Dwarf_Unsigned* /*sec_size*/,
    Dwarf_Error *   /*err*/);

DW_API void dwarf_xu_header_free(Dwarf_Xu_Index_Header /*xuhdr*/);
/*  For any Dwarf_Die in a compilation unit, return
    the debug fission table data through
    percu_out.   Usually applications
    will pass in the CU die.
    Calling code should zero all of the
    struct Dwarf_Debug_Fission_Per_CU_s before calling this.
    If there is no debugfission data this returns
    DW_DLV_NO_ENTRY (only .dwp objects have debugfission data).  */
DW_API int dwarf_get_debugfission_for_die(Dwarf_Die /* die */,
    Dwarf_Debug_Fission_Per_CU * /* percu_out */,
    Dwarf_Error * /* err */);

/*  Given a key (hash signature)  from a .o,
    find the per-cu information
    for the CU with that key. */
DW_API int dwarf_get_debugfission_for_key(Dwarf_Debug /*dbg*/,
    Dwarf_Sig8 *                 /*key, hash signature */,
    const char * key_type        /*"cu" or "tu" */,
    Dwarf_Debug_Fission_Per_CU * /*percu_out */,
    Dwarf_Error *                /*err */);

/*  END debugfission dwp .debug_cu_index
    and .debug_tu_index operations. */

/*! @} */

/*! @defgroup gnudebuglink Access GNU .gnu_debuglink, build-id.
    When DWARF is separate from a normal shared object.
    Has nothing to do with split-dwarf/debug-fission.
    @{
*/

/*  New October 2019.  Access to the GNU section named
    .gnu_debuglink  and/or the section .note.gnu.build-id.
    See
    https://sourceware.org/gdb/onlinedocs/gdb/
        Separate-Debug-Files.html

    If no debuglink then name_returned,crc_returned and
    debuglink_path_returned will get set 0 through the pointers.

    If no .note.gnu.build-id then  buildid_length_returned,
    and buildid_returned will be set 0 through the pointers.

    Caller frees space returned by debuglink_fullpath_returned.

    See libdwarf2.1.mm revision 3.13 or later for
    additional important details.
*/

DW_API int dwarf_gnu_debuglink(Dwarf_Debug /*dbg*/,
    char     **    /*debuglink_path_returned */,
    unsigned char ** /*crc_returned from the debuglink section*/,
    char     **    /*debuglink_fullpath_returned free this*/,
    unsigned int * /*debuglink_path_count_returned */,
    unsigned int * /*buildid_type_returned */,
    char     **    /*buildid_owner_name_returned*/,
    unsigned char ** /*buildid_returned*/,
    unsigned int * /*buildid_length_returned*/,
    char     ***   /*paths_returned*/,
    unsigned int * /*paths_length_returned*/,
    Dwarf_Error*   /*error*/);

/*  Only useful inside dwarfexample/dwdebuglink.c
    so we can show all that is going on.
*/
DW_API int dwarf_add_debuglink_global_path(Dwarf_Debug /*dbg*/,
    const char *pathname,
    Dwarf_Error* /*error*/);

/*  crc32 used for debuglink crc calculation.
    Caller passes pointer to array of 4 unsigned char
    provided by the caller and if this returns
    DW_DLV_OK that is filled in. */
DW_API int dwarf_crc32(Dwarf_Debug /*dbg*/,
    unsigned char * /*crcbuf*/,
    Dwarf_Error * /*error*/);

/*  Public interface to the real crc calculation
    just in case some find it useful. */
DW_API unsigned int dwarf_basic_crc32(const unsigned char * /*buf*/,
    unsigned long /*len*/, unsigned int /*init*/);
/*! @} */
/*! @defgroup harmless Harmless Error recording

    The harmless error list is a circular buffer of
    errors we note but which do not stop us from processing
    the object.  Created so dwarfdump or other tools
    can report such inconsequential errors without causing
    anything to stop early.
    @{
*/
/*! @brief  Default size of the libdwarf-internal circular list */
#define DW_HARMLESS_ERROR_CIRCULAR_LIST_DEFAULT_SIZE 4

/*! @brief Get the harmless error count and content

    User code supplies size of array of pointers dw_errmsg_ptrs_array
    in count and the array of pointers (the pointers themselves
    need not be initialized).
    The pointers returned in the array of pointers
    are invalidated by ANY call to libdwarf.
    Use them before making another libdwarf call!
    The array of string pointers passed in always has
    a final null pointer, so if there are N pointers the
    and M actual strings, then MIN(M,N-1) pointers are
    set to point to error strings.  The array of pointers
    to strings always terminates with a NULL pointer.
    Do not free the strings.  Every string is null-terminated.

    Each call empties the error list (discarding all current entries).
    and fills in your array

    @param dw_dbg
    The applicable Dwarf_Debug.
    @param dw_count
    The number of string buffers.
    If count is passed as zero no elements of the array are touched.
    @param dw_errmsg_ptrs_array
    A pointer to a user-created array of
    pointer to const char.
    @param dw_newerr_count
    If non-NULL the count of harmless errors
    pointers since the last call is returned through the pointer.
    If dw_count is greater than zero the first dw_count
    of the pointers in the user-created array point
    to null-terminated strings. Do not free the strings.
    print or copy the strings before any other libdwarf call.

    @return
    Returns DW_DLV_NO_ENTRY if no harmless errors
    were noted so far.  Returns DW_DLV_OK if there are
    harmless errors.  Never returns DW_DLV_ERROR.

    If DW_DLV_NO_ENTRY is returned none of the arguments
    other than dw_dbg are touched or used.
    */
DW_API int dwarf_get_harmless_error_list(Dwarf_Debug dw_dbg,
    unsigned int   dw_count,
    const char **  dw_errmsg_ptrs_array,
    unsigned int * dw_newerr_count);

/*! @brief The size of the circular list of strings
    libdwarf holds internally may be set
    and reset as needed.  If it is shortened excess
    messages are simply dropped.  It returns the previous
    size. If zero passed in the size is unchanged
    and it simply returns the current size.

    @param dw_dbg
    The applicable Dwarf_Debug.
    @param dw_maxcount
    Set the new internal buffer count to a number greater
    than zero.
    @return
    returns the current size of the internal circular
    buffer if dw_maxcount is zero.
    If dw_maxcount is greater than zero the internal
    array is adjusted to hold that many and the
    previous number of harmless errors possible in
    the circular buffer is returned.
    */
DW_API unsigned int dwarf_set_harmless_error_list_size(
    Dwarf_Debug  dw_dbg,
    unsigned int dw_maxcount);

/*  Insertion is only for testing the harmless error code, it is not
    useful otherwise. */
DW_API void dwarf_insert_harmless_error(Dwarf_Debug dw_dbg,
    char * dw_newerror);
/*! @} */

/*! @defgroup Naming Names DW_TAG AT etc as strings
    @{
*/
/*  When the val_in is known these, for example, dwarf_get_TAG_name
    functions return the string corresponding to the val_in passed in
    through the pointer s_out and the value returned is DW_DLV_OK.
    The strings are in static storage
    and must not be freed.
    If DW_DLV_NO_ENTRY is returned the val_in is not known and
    *s_out is not set.  DW_DLV_ERROR is never returned.*/

/* The following copied from a generated dwarf_names.h */

/*  dwarf_get_LLEX_name is for a GNU extension.
    Not defined by the GNU folks nor a DWARF standard
    but it seemed essential. */
/* BEGIN FILE */
DW_API int dwarf_get_ACCESS_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_ADDR_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_AT_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_ATCF_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_ATE_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_CC_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_CFA_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_children_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_CHILDREN_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_DEFAULTED_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_DS_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_DSC_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_EH_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_END_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_FORM_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_FRAME_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_GNUIKIND_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_GNUIVIS_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_ID_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_IDX_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_INL_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_ISA_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_LANG_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_LLE_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_LLEX_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_LNCT_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_LNE_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_LNS_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_MACINFO_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_MACRO_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_OP_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_ORD_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_RLE_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_SECT_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_TAG_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_UT_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_VIRTUALITY_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
DW_API int dwarf_get_VIS_name(unsigned int /*val_in*/,
    const char ** /*s_out */);
/* END FILE */

DW_API int dwarf_get_FORM_CLASS_name(enum Dwarf_Form_Class /*fc*/,
    const char ** /*s_out*/);
/*! @} */

/*! @defgroup objectsections Object Sections Data
    @{
*/
/*  Section name access.  Because sections might
    now end with .dwo or be .zdebug  or might not.
*/
DW_API int dwarf_get_die_section_name(Dwarf_Debug /*dbg*/,
    Dwarf_Bool    /*is_info*/,
    const char ** /*sec_name*/,
    Dwarf_Error * /*error*/);

DW_API int dwarf_get_die_section_name_b(Dwarf_Die /*die*/,
    const char ** /*sec_name*/,
    Dwarf_Error * /*error*/);

DW_API int dwarf_get_macro_section_name(Dwarf_Debug /*dbg*/,
    const char ** /*sec_name_out*/,
    Dwarf_Error * /*err*/);

DW_API int dwarf_get_real_section_name(Dwarf_Debug /*dbg*/,
    const char * /*std_section_name*/,
    const char ** /*actual_sec_name_out*/,
    Dwarf_Small * /*marked_compressed*/,  /* .zdebug... */
    Dwarf_Small * /*marked_zlib_compressed */, /* ZLIB string */
    Dwarf_Small * /*marked_shf_compressed*/, /* SHF_COMPRESSED */
    Dwarf_Unsigned * /*compressed_length*/,
    Dwarf_Unsigned * /*uncompressed_length*/,
    Dwarf_Error * /*error*/);

DW_API int dwarf_get_frame_section_name(Dwarf_Debug /*dbg*/,
    const char ** /*section_name_out*/,
    Dwarf_Error * /*error*/);

DW_API int dwarf_get_frame_section_name_eh_gnu(Dwarf_Debug /*dbg*/,
    const char ** /*section_name_out*/,
    Dwarf_Error * /*error*/);
DW_API int dwarf_get_aranges_section_name(Dwarf_Debug /*dbg*/,
    const char ** /*section_name_out*/,
    Dwarf_Error * /*error*/);
DW_API int dwarf_get_ranges_section_name(Dwarf_Debug /*dbg*/,
    const char ** /*section_name_out*/,
    Dwarf_Error * /*error*/);

/*  These two get the offset or address size as defined
    by the object format (not by DWARF). */
DW_API int dwarf_get_offset_size(Dwarf_Debug /*dbg*/,
    Dwarf_Half  *    /*offset_size*/,
    Dwarf_Error *    /*error*/);
DW_API int dwarf_get_address_size(Dwarf_Debug /*dbg*/,
    Dwarf_Half  *    /*addr_size*/,
    Dwarf_Error *    /*error*/);

DW_API int dwarf_get_string_section_name(Dwarf_Debug /*dbg*/,
    const char ** /*section_name_out*/,
    Dwarf_Error * /*error*/);

DW_API int dwarf_get_line_section_name(Dwarf_Debug /*dbg*/,
    const char ** /*section_name_out*/,
    Dwarf_Error * /*error*/);
DW_API int dwarf_get_line_section_name_from_die(Dwarf_Die /*die*/,
    const char ** /*section_name_out*/,
    Dwarf_Error * /*error*/);

/* Giving a section name, get its size and address */
DW_API int dwarf_get_section_info_by_name(Dwarf_Debug /*dbg*/,
    const char *     /*section_name*/,
    Dwarf_Addr*      /*section_addr*/,
    Dwarf_Unsigned*  /*section_size*/,
    Dwarf_Error*     /*error*/);

/* Giving a section index, get its size and address */
DW_API int dwarf_get_section_info_by_index(Dwarf_Debug /*dbg*/,
    int              /*section_index*/,
    const char **    /*section_name*/,
    Dwarf_Addr*      /*section_addr*/,
    Dwarf_Unsigned*  /*section_size*/,
    Dwarf_Error*     /*error*/);

/*  Get section count, of object file sections. */
DW_API int dwarf_get_section_count(Dwarf_Debug /*dbg*/);

/*  This is a hack so clients can verify offsets.
    Added so that one can detect broken offsets
    (which happened in an IRIX executable larger than 2GB
    with MIPSpro 7.3.1.3 toolchain.).
    This has 21 arguments, which is...unusual.
*/
DW_API int dwarf_get_section_max_offsets_d(Dwarf_Debug /*dbg*/,
    Dwarf_Unsigned * /*debug_info_size*/,
    Dwarf_Unsigned * /*debug_abbrev_size*/,
    Dwarf_Unsigned * /*debug_line_size*/,
    Dwarf_Unsigned * /*debug_loc_size*/,
    Dwarf_Unsigned * /*debug_aranges_size*/,

    Dwarf_Unsigned * /*debug_macinfo_size*/,
    Dwarf_Unsigned * /*debug_pubnames_size*/,
    Dwarf_Unsigned * /*debug_str_size*/,
    Dwarf_Unsigned * /*debug_frame_size*/,
    Dwarf_Unsigned * /*debug_ranges_size*/,

    Dwarf_Unsigned * /*debug_pubtypes_size*/,
    Dwarf_Unsigned * /*debug_types_size*/,
    Dwarf_Unsigned * /*debug_macro_size*/,
    Dwarf_Unsigned * /*debug_str_offsets_size*/,
    Dwarf_Unsigned * /*debug_sup_size*/,

    Dwarf_Unsigned * /*debug_cu_index_size*/,
    Dwarf_Unsigned * /*debug_tu_index_size*/,
    Dwarf_Unsigned * /*debug_names_size*/,
    Dwarf_Unsigned * /*debug_loclists_size*/,
    Dwarf_Unsigned * /*debug_rnglists_size*/);

/*  Allows callers to find out what groups (dwo or COMDAT)
    are in the object and how much to allocate so one can get the
    group-section map data. */
DW_API int dwarf_sec_group_sizes(Dwarf_Debug /*dbg*/,
    Dwarf_Unsigned * /*section_count_out*/,
    Dwarf_Unsigned * /*group_count_out*/,
    Dwarf_Unsigned * /*selected_group_out*/,
    Dwarf_Unsigned * /*map_entry_count_out*/,
    Dwarf_Error    * /*error*/);

/*  New May 2017. Reveals the map between group numbers and
    section numbers.
    Caller must allocate the arrays with space for 'map_entry_count'
    values and this function fills in the array entries.
    Output ordered by group number and section number.
    */
DW_API int dwarf_sec_group_map(Dwarf_Debug /*dbg*/,
    Dwarf_Unsigned   /*map_entry_count*/,
    Dwarf_Unsigned * /*group_numbers_array*/,
    Dwarf_Unsigned * /*sec_numbers_array*/,
    const char    ** /*sec_names_array*/,
    Dwarf_Error    * /*error*/);
/*! @} */

/*! @defgroup leb LEB Encode and Decode
    @{
*/
DW_API int dwarf_encode_leb128(Dwarf_Unsigned /*val*/,
    int * /*nbytes*/,
    char * /*space*/,
    int /*splen*/);
DW_API int dwarf_encode_signed_leb128(Dwarf_Signed /*val*/,
    int * /*nbytes*/,
    char * /*space*/,
    int /*splen*/);
/*  Same for LEB decoding routines.
    caller sets endptr to an address one past the last valid
    address the library should be allowed to
    access. */
DW_API int dwarf_decode_leb128(char * /*leb*/,
    Dwarf_Unsigned * /*leblen*/,
    Dwarf_Unsigned * /*outval*/,
    char           * /*endptr*/);
DW_API int dwarf_decode_signed_leb128(char * /*leb*/,
    Dwarf_Unsigned * /*leblen*/,
    Dwarf_Signed   * /*outval*/,
    char           * /*endptr*/);
/*! @} */

/*! @defgroup miscellaneous Miscellaneous Functions
    @{
*/
/*! @brief Returns the version string in the library. 

    An example: "0.3.0" which is a Semantic Version identifier.
    Before September 2021 the version string was
    a date, for example "20210528",
    which is in ISO date format.
    See DW_LIBDWARF_VERSION DW_LIBDWARF_VERSION_MAJOR
    DW_LIBDWARF_VERSION_MINOR DW_LIBDWARF_VERSION_MICRO 
    @return
    The Package Version built into libdwarf.so or libdwarf.a
*/
DW_API const char * dwarf_package_version(void);


/*! @brief  Turn off libdwarf checks of strings.

    Zero is  the default and means do all
    string length validity checks.
    It applies to all Dwarf_Debug open and
    all opened later in this library instance.

    @param dw_stringcheck
    Pass in a small non-zero value to turn off
    all libdwarf string validity checks. It speeds
    up libdwarf, but...is dangerous and voids
    all promises the library will not segfault.
    @return
    Returns the previous value of this flag.
*/
DW_API int dwarf_set_stringcheck(int dw_stringcheck);

/*! @brief Set libdwarf response to *.rela relocations

    dw_apply defaults to 1 and means apply all
    '.rela' relocations on reading in a dwarf object
    section of such relocations.
    Best to just ignore this function
    It applies to all Dwarf_Debug open and
    all opened later in this library instance.

    @param dw_apply
    Pass in a zero to turn off reading and applying of
    *.rela relocations, which will likely break reading
    of .o object files but probably will not break reading
    executables or shared objects.
    Pass in non zero (it is really just an 8 bit value,
    so use a small value) to turn off inspecting .rela
    sections.
    @return
    Returns the previous value of the apply flag.

*/
DW_API int dwarf_set_reloc_application(int dw_apply);

/*  dwarf_get_endian_copy_function new. December 2019. */
DW_API void (*dwarf_get_endian_copy_function(Dwarf_Debug /*dbg*/))
    (void *, const void * /*src*/, unsigned long /*srclen*/);

/*  A global flag in libdwarf. Applies to all Dwarf_Debug */
DW_API extern Dwarf_Cmdline_Options dwarf_cmdline_options;

/*! @brief Tell libdwarf to add verbosity to Line Header errors
    By default the flag in the struct argument
    is zero.
    dwarfdump uses this when -v used on dwarfdump.

    @see dwarf_register_printf_callback

    @param dw_dd_options
    The structure has one flag, and if
    the flag is nonzero and there is an error
    in reading a line table header
    the function passes back detail error messages
    via dwarf_register_printf_callback.
*/
DW_API void dwarf_record_cmdline_options(
    Dwarf_Cmdline_Options dw_dd_options);

/*!  @brief Eliminate libdwarf tracking of allocations
    Independent of any Dwarf_Debug and applicable
    to all whenever the setting is changed.
    Defaults to non-zero.

    @param dw_v
    If zero passed in libdwarf will run somewhat faster
    and  library memory allocations
    will not all be tracked and dwarf_finish() will
    be unable to free/dealloc some things.
    User code can do the necessary deallocs
    (as documented), but the normal guarantee
    that libdwarf will clean up is revoked.
    If non-zero passed in libdwarf will resume or
    continue tracking allocations
    @return
    Returns the previous version of the flag.
*/
DW_API int dwarf_set_de_alloc_flag(int dw_v);

/*! @brief Set the address size on a Dwarf_Debug

    DWARF information CUs and other
    section DWARF headers define a CU-specific
    address size, but this Dwarf_Debug value
    is used when other address size information
    does not exist, for example in a DWARF2
    CIE or FDE.
 
    @param dw_dbg
    The Dwarf_Debug of interest.
    @param dw_value
    Sets the address size for the Dwarf_Debug to a
    non-zero value. The default address size
    is derived from headers in the object file.
    Values larger than the size of Dwarf_Addr
    are not set.
    If zero passed the default is not changed.
    @return
    Returns the last set address size.
*/
DW_API Dwarf_Small dwarf_set_default_address_size(
    Dwarf_Debug dw_dbg,
    Dwarf_Small dw_value);


/*! @}
*/

/*! @defgroup objectdetector Determine Object Type of a File
    @{
*/
DW_API int dwarf_object_detector_path_b(const char * /*path*/,
    char         *   /* outpath_buffer*/,
    unsigned long    /* outpathlen*/,
    char **          /* gl_pathnames*/,
    unsigned int     /* gl_pathcount*/,
    unsigned int *   /* ftype*/,
    unsigned int *   /* endian*/,
    unsigned int *   /* offsetsize*/,
    Dwarf_Unsigned * /* filesize*/,
    unsigned char *  /* pathsource*/,
    int * /*errcode*/);

/* Solely looks for dSYM */
DW_API int dwarf_object_detector_path_dSYM(const char * /*path*/,
    char *         /* outpath*/,
    unsigned long  /* outpath_len*/,
    char **        /* gl_pathnames*/,
    unsigned int   /* gl_pathcount*/,
    unsigned int * /* ftype*/,
    unsigned int * /* endian*/,
    unsigned int * /* offsetsize*/,
    Dwarf_Unsigned  * /* filesize*/,
    unsigned char  *  /* pathsource*/,
    int *             /* errcode*/);

DW_API int dwarf_object_detector_fd(int /*fd*/,
    unsigned int * /*ftype*/,
    unsigned int * /*endian*/,
    unsigned int * /*offsetsize*/,
    Dwarf_Unsigned  * /*filesize*/,
    int *  /*errcode*/);

/*! @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _LIBDWARF_H */
