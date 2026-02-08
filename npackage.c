/******************************************************************
* Copyright (C) 2026 NLABS -- William J. <williamj.inbox@gmail.com>
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program; if not, see https://www.gnu.org/licenses/
*******************************************************************/
#include "npackage.h"

npackage *init_npackage(void)
{
    npackage *p = (npackage*)malloc(sizeof(npackage));
    p->make_time = time(NULL);
    p->mod_time = 0;
    p->mod_count = 0;
    p->asset_count = 0;
    p->sizes = NULL;
    p->assets = NULL;
    return p;
}

uint8_t npackage_has_key(npackage *p, const wchar_t *k)
{
    for ( uint64_t q = 0; q < p->asset_count; q++ ) {
        if ( wcscmp(p->assets[q].key, k) == 0 )
            return 1;
    }
    return 0;
}

uint64_t npackage_header_size(npackage *p)
{
    uint64_t s = 0;
    /* + make_time, mod_time, mod_count, asset_count */
    s+= sizeof(uint64_t) * 4;
    /* += sizes = (asset_count * uint64_t) */
    s+= sizeof(uint64_t) * p->asset_count;
    s+= sizeof(nasset*) * p->asset_count;
    return s;
}

uint8_t npackage_save(const wchar_t *fp, npackage *p)
{
    FILE *fh = _wfopen(fp, L"wb");
    if ( fh == NULL )
        return 0;
    uint8_t sig[7] = {110, 108, 97, 98, 115, 110, 112}; 
    uint8_t t1 = fwrite(sig, sizeof(uint8_t), 7, fh) == 7;
    uint8_t t2 = fwrite(&p->make_time, sizeof(uint64_t), 1, fh) == 1;
    uint8_t t3 = fwrite(&p->mod_time, sizeof(uint64_t), 1, fh) == 1;
    uint8_t t4 = fwrite(&p->mod_count, sizeof(uint64_t), 1, fh) == 1;
    uint8_t t5 = fwrite(&p->asset_count, sizeof(uint64_t), 1, fh) == 1;
    uint8_t t6 = fwrite(p->sizes, sizeof(uint64_t), p->asset_count, fh) == p->asset_count;
    uint64_t t7 = 0;
    nasset *a;
    for ( uint64_t k = 0; k < p->asset_count; k++ ) {
        a = &p->assets[k];
        t7 += fwrite(&p->assets[k].make_time, sizeof(uint64_t), 1, fh);
        t7 += fwrite(&p->assets[k].mod_time, sizeof(uint64_t), 1, fh);
        t7 += fwrite(&p->assets[k].mod_count, sizeof(uint64_t), 1, fh);
        t7 += fwrite(&p->assets[k].data_len, sizeof(uint64_t), 1, fh);
        t7 += fwrite(&p->assets[k].encryption, sizeof(uint8_t), 1, fh);
        t7 += fwrite(&p->assets[k].compression, sizeof(uint8_t), 1, fh);
        t7 += fwrite(&p->assets[k].key_len, sizeof(uint64_t), 1, fh);
        t7 += fwrite(p->assets[k].key, sizeof(wchar_t), p->assets[k].key_len, fh);
        t7 += fwrite(p->assets[k].data, sizeof(char), p->assets[k].data_len, fh);
        t7 = t7 == (7 + p->assets[k].key_len + p->assets[k].data_len);
        if ( t7 != 1 )
            break;
    }
    fclose(fh);
    if ( t1 && t2 && t3 && t4 && t5 && t6 && t7 )
        return 1;
    return 0;
}

uint8_t nasset_compression(nasset *a)
{
    return a->compression;
}

nasset * nasset_from_disk(const wchar_t *k, const wchar_t *fp)
{
    nasset *a = init_nasset();
    FILE *fh = _wfopen(fp, L"rb");
    uint64_t len;
    if ( !fh ) {
        free(a);
        return NULL;
    }
    fseek(fh, 0, SEEK_END);
    len = ftell(fh);
    rewind(fh);
    a->data = (unsigned char*)malloc(len);
    if ( !a->data ) {
        fclose(fh);
        free(a);
        return NULL;
    }
    if ( len != fread(a->data, sizeof(unsigned char), len, fh) ) {
        fclose(fh);
        free(a);
        return NULL;
    }
    fclose(fh);
    a->data_len = len;
    a->key_len = sizeof(wchar_t) * wcslen(k);
    a->key = k;
    return a;
}

nasset *nasset_get(npackage *p, const wchar_t *k)
{
    for ( uint64_t q = 0; q < p->asset_count; q++ ) {
        if ( wcscmp(p->assets[q].key, k) == 0 )
            return &p->assets[q];
    }
    return NULL;
}

nasset *init_nasset(void)
{
    nasset *a = (nasset*)malloc(sizeof(nasset));
    a->package = NULL;
    a->make_time = time(NULL);
    a->mod_time = 0;
    a->mod_count = 0;
    a->data_len = 0;
    a->encryption = 0;
    a->compression = 0;
    a->key = NULL;
    a->key_len = 0;
    a->data = NULL;
    return a;
}

uint8_t nasset_set_key(nasset *a, const wchar_t *k)
{
    npackage* p = (npackage*)a->package;
    if ( p != NULL ) {
        if ( npackage_has_key(p, k) )
            return -1;
    }
    if ( a->key != NULL )
    {
        free(a->key);
        /*
        modifications are only tracked
        after a key is first assigned
        */
        a->mod_count++;
        a->mod_time = time(NULL);
        if ( p != NULL )
        {
            p->mod_time = a->mod_time;
            p->mod_count++;
        }
    }
    a->key_len = sizeof(wchar_t) * wcslen(k);
    a->key = k;
    return 1;
}

uint8_t nasset_set_value(nasset *a, unsigned char *v, uint64_t len)
{
    /*
    mods only tracked after asset has been set 
    */
    if ( a->key != NULL && a->data != NULL )
    {
        a->mod_count++;
        a->mod_time = time(NULL);
    }

    npackage* p = (npackage*)a->package;
    a->data = v;
    a->data_len = len;

    /*
    p will be equal=null if no
    insertion has taken place
    */
    if ( p != NULL )
    {
        p->mod_time = a->mod_time;
        p->mod_count++;
    }
    return 1;
}

uint64_t nasset_size(nasset *a)
{
    uint64_t s = 0;
    /* + make_time, mod_time, mod_count, length */
    s += sizeof(uint64_t) * 4;
    /* + encryption, compression... */
    s += sizeof(uint8_t) * 2;
    /* + key size... */
    s += sizeof(uint64_t);
    /* + key size */
    if ( a->key != NULL )
        s += a->key_len;
    /* + data size */
    s += sizeof(char) * a->data_len;
    return s;
}

uint64_t nassets_size(npackage *p)
{
    uint64_t s = 0;
    for ( uint64_t k = 0; k < p->asset_count; k++ )
        s += nasset_size(&p->assets[k]);
    return s;
}

uint8_t nasset_insert(npackage *p, nasset *a)
{
    p->asset_count++;

    uint64_t *ps;
    if ( p->sizes != NULL )
        ps = (uint64_t*)realloc(p->sizes, p->asset_count * sizeof(uint64_t));
    else
        ps = (uint64_t*)malloc(sizeof(uint64_t));
    if ( ps == NULL )
        return 0;
    p->sizes = ps;
    p->sizes[p->asset_count-1] = nasset_size(a);

    nasset *pa;
    if ( p->assets != NULL )
        pa = (nasset*)realloc(p->assets, p->asset_count * sizeof(nasset));
    else
        pa = (nasset*)malloc(sizeof(nasset));
    if ( pa == NULL )
        return 0;
    p->assets = pa;
    p->assets[p->asset_count-1] = *a;

    p->mod_time = time(NULL);
    p->mod_count++;
    a->package = p;
    return 1;
}

uint64_t npackage_size(npackage *p)
{
    uint64_t s = 0;
    /* + make_time, mod_time, mod_count, asset_count */
    s+= sizeof(uint64_t) * 4;
    /* += sizes = (asset_count * uint64_t) */
    s+= sizeof(uint64_t) * p->asset_count;
    /* += nasset_size */
    s+= sizeof(nasset*) * p->asset_count;
    for ( uint64_t k = 0; k < p->asset_count; k++ )
        s += nasset_size(&p->assets[k]);
    return s;
}
