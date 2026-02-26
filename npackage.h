/********************************************************************
* Copyright (C) 2026 NLABS -- William J. <williamj.inbox@gmail.com>
* 
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
* 02110-1301 USA
********************************************************************/
#ifndef _NLABS_NP
#define _NLABS_NP
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include "utils.h"

/*
replacement for wchar_t (2 bytes on win)
*/
typedef uint32_t nwide_c;

/*
nasset's are nested inside npackages
*/
typedef struct
{
    /*
    pointer to parent
    */
    void *package;

    /*
	assembly epoch
	*/
	uint64_t make_time;

    /*
	last modification time
	*/
	uint64_t mod_time;

    /*
	modifications count of the package
	*/
	uint64_t mod_count;

    /*
    length of raw data in bytes
    */
    uint64_t data_len;

	/*
	length of key in bytes
	*/
	uint64_t key_len;

    /*
    key ident
    */
    nwide_c *key;

    /*
    raw data
    */
    unsigned char *data;
}
nasset;

/*
npackage's contain a number of nassets
*/
typedef struct
{
	/*
	assembly epoch
	*/
	uint64_t make_time;

	/*
	last modification time
	*/
	uint64_t mod_time;

	/*
	modifications count of the package
	*/
	uint64_t mod_count;

	/*
	encryption id
	*/
	uint8_t encryption;

	/*
	compression id
	*/
	uint8_t compression;

	/*
	number of enclosed assets
	*/
	uint64_t asset_count;

	/*
	sizes of each enclosed asset
	*/
	uint64_t *sizes;

	/*
	dataset assets
	*/
	nasset **assets;
}
npackage;

/*
assemble an empty asset
*/
nasset *new_nasset(void);

/*
nasset comparison
*/
int8_t nasset_cmp(nasset *p, nasset *q);

/*
delete the asset from the package
*/
uint8_t nasset_delete(npackage *p, const wchar_t *k);

/*
load asset from disk
*/
nasset * nasset_from_local(const wchar_t *k, const wchar_t *fp);

/*
get/fetch asset from package p given a key k
*/
nasset *nasset_get(npackage *p, const wchar_t *k);

/*
insert an asset a into a package p
*/
uint8_t nasset_insert(npackage *p, nasset *a);

/*
size of asset a
*/
uint64_t nasset_size(nasset *a);

/*
size of all assets
*/
uint64_t nassets_size(npackage *p);

/*
close nasset
*/
uint8_t nasset_close(nasset *a);

/*
unset the asset from the package
*/
nasset *nasset_unset(npackage *p, const wchar_t *k);

/*
assemble an empty package
*/
npackage *new_npackage(void);

/*
close the package
*/
uint8_t npackage_close(npackage *p);

/*
signals whether package p is compressed
*/
uint8_t npackage_compression(npackage *p);

/*
signals whether package p is encrypted
*/
uint8_t npackage_encryption(npackage *p);

/*
queries whether a given key exists in the heap
*/
uint8_t npackage_has_key(npackage *p, const wchar_t *k);

/*
size of header without attached assets
*/
uint64_t npackage_header_size(npackage *p);

/*
load package from disk file
*/
npackage *npackage_open(const wchar_t *fp);

/*
flush package into disk file
*/
uint8_t npackage_save(const wchar_t *fp, npackage *p);

/*
size of npackage
*/
uint64_t npackage_size(npackage *p);

/*
set an assets (key, value) given an asset a
- the existing key should it exist will be freed
- the existing value should it exist will be freed
*/
uint8_t nasset_set_key(nasset *a, const wchar_t *k);
uint8_t nasset_set_value(nasset *a, unsigned char *v, uint64_t len);

#endif // _NLABS_NP