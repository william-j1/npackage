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
#include "npackage.h"

npackage *new_npackage(void)
{
    npackage *p = (npackage*)malloc(sizeof(npackage));
    p->make_time = time(NULL);
    p->mod_time = 0;
    p->mod_count = 0;
    p->asset_count = 0;
    p->encryption = 0;
    p->compression = 0;
    p->sizes = NULL;
    p->assets = NULL;
    return p;
}

uint8_t npackage_has_key(npackage *p, const wchar_t *k)
{
    for ( uint64_t q = 0; q < p->asset_count; q++ ) {
        if ( wcscmp(p->assets[q]->key, k) == 0 )
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

npackage * npackage_open(const wchar_t *fp)
{
	FILE *fh;
	npackage *p = new_npackage();
    uint8_t sig[7] = {110, 108, 97, 98, 115, 110, 112};
    uint8_t fsig[7] = {0, 0, 0, 0, 0, 0, 0};

    /*
    endian flags
    */
    uint8_t _be = is_be(), _eibe;

	fh = _wfopen(fp, L"rb");
    if ( fh == NULL )
        return NULL;

    /*
    extract and compare signature    
    */
    uint8_t t1 = fread(fsig, sizeof(uint8_t), 7, fh) == 7;
    if ( !t1 ) { /* ... if less than 7 bytes read */
        fclose(fh);
        return NULL;
    }
    if ( arrcmp(sig, fsig, 7) != 0 ) {
        fclose(fh);
        return NULL;
    }

    /*
    size of current wide
    */
    uint8_t _szcw = sizeof(wchar_t);

    /*
    copy of size of wide during _export_
    */
    uint8_t _szwc;

    /*
    encoded in big endian flag
    */
    uint8_t t10 = fread(&_eibe, sizeof(uint8_t), 1, fh) == 1;

    /*
    extract the size of the wide char on the export system
    if the current wide char size is less than, terminate
    */
    uint8_t t11 = fread(&_szwc, sizeof(uint8_t), 1, fh) == 1;
    if ( _szcw < _szwc )
    {
        fclose(fh);
        return NULL;
    }

    uint8_t t2 = fread(&p->make_time, sizeof(uint64_t), 1, fh) == 1;
    uint8_t t3 = fread(&p->mod_time, sizeof(uint64_t), 1, fh) == 1;
    uint8_t t4 = fread(&p->mod_count, sizeof(uint64_t), 1, fh) == 1;
    uint8_t t8 = fread(&p->encryption, sizeof(uint8_t), 1, fh) == 1;
    uint8_t t9 = fread(&p->compression, sizeof(uint8_t), 1, fh) == 1;
    uint8_t t5 = fread(&p->asset_count, sizeof(uint64_t), 1, fh) == 1;

    /*
    adjust endianness
    */
    if ( _be != _eibe )
    {
        p->make_time = u64swap_endian(p->make_time);
        p->mod_time = u64swap_endian(p->mod_time);
        p->mod_count = u64swap_endian(p->mod_count);
        p->asset_count = u64swap_endian(p->asset_count);
    }

    p->sizes = (uint64_t*)malloc(sizeof(uint64_t) * p->asset_count);
    p->assets = (nasset**)malloc(sizeof(nasset) * p->asset_count);
    uint8_t t6 = fread(p->sizes, sizeof(uint64_t), p->asset_count, fh) == p->asset_count;

    uint64_t t7 = 0;
    int64_t k;
    for ( k = 0; k < p->asset_count; k++ )
    {
        nasset *a = new_nasset();
        t7 += fread(&a->make_time, sizeof(uint64_t), 1, fh);
        t7 += fread(&a->mod_time, sizeof(uint64_t), 1, fh);
        t7 += fread(&a->mod_count, sizeof(uint64_t), 1, fh);
        t7 += fread(&a->data_len, sizeof(uint64_t), 1, fh);
        t7 += fread(&a->key_len, sizeof(uint64_t), 1, fh);

        a->key = (wchar_t*)malloc(sizeof(wchar_t) * a->key_len);
        t7 += fread(a->key, sizeof(wchar_t), a->key_len, fh);
        a->data = (unsigned char*)malloc(sizeof(char) * a->data_len);
        t7 += fread(a->data, sizeof(char), a->data_len, fh);

        /*
        integrity check
        */
        t7 = t7 == (5 + a->key_len + a->data_len);
        if ( t7 != 1 )
        {
            if ( a->key != NULL )
                free(a->key);
            if ( a->data != NULL )
                free(a->data);
            free(a);
            break;
        }

        /*
        change endian 
        */
        if ( _be != _eibe )
        {
            a->make_time = u64swap_endian(a->make_time);
            a->mod_time = u64swap_endian(a->mod_time);
            a->mod_count = u64swap_endian(a->mod_count);
            a->data_len = u64swap_endian(a->data_len);
            a->key_len = u64swap_endian(a->key_len);
            p->sizes[k] = u64swap_endian(p->sizes[k]);
            for ( uint64_t j = 0; j < a->key_len; j++ )
            {
                if ( _szcw == 2 )
                    a->key[j] = (a->key[j] << 8) | (a->key[j] >> 8);
                else if ( _szcw == 4 )
                    a->key[j] = (a->key[j] << 16) | (a->key[j] >> 16);
            }
        }

        /*
        attach asset to parent
        and seat asset to index
        */
        a->package = p;
        p->assets[k] = a;
    }
    fclose(fh);

    /*
    ... package
    */
    if ( t1 && t2 && t3 && t4 && t5 && t6 && t7 && t8 && t9 && t10 && t11 )
        return p;

    /*
    ... integrity fail, cleanse structs
    */
    while ( k >= 0 ) {
        nasset *ah = p->assets[k];
        if ( ah->key != NULL )
            free(ah->key);
        if ( ah->data != NULL )
            free(ah->data);
        free(ah);
        k--;
    }
    free(p->assets);
    free(p->sizes);
    return NULL;
}

uint8_t npackage_save(const wchar_t *fp, npackage *p)
{
    FILE *fh = _wfopen(fp, L"wb");
    if ( fh == NULL )
        return 0;
    uint8_t sig[7] = {110, 108, 97, 98, 115, 110, 112};
    uint8_t _be = is_be();

    /*
    ... either 2 or 4 bytes
    */
    uint8_t _szws = sizeof(wchar_t);

    uint8_t t1 = fwrite(sig, sizeof(uint8_t), 7, fh) == 7;
    uint8_t t10 = fwrite(&_be, sizeof(uint8_t), 1, fh) == 1;
    uint8_t t11 = fwrite(&_szws, sizeof(uint8_t), 1, fh) == 1;
    uint8_t t2 = fwrite(&p->make_time, sizeof(uint64_t), 1, fh) == 1;
    uint8_t t3 = fwrite(&p->mod_time, sizeof(uint64_t), 1, fh) == 1;
    uint8_t t4 = fwrite(&p->mod_count, sizeof(uint64_t), 1, fh) == 1;
    uint8_t t8 = fwrite(&p->encryption, sizeof(uint8_t), 1, fh) == 1;
    uint8_t t9 = fwrite(&p->compression, sizeof(uint8_t), 1, fh) == 1;
    uint8_t t5 = fwrite(&p->asset_count, sizeof(uint64_t), 1, fh) == 1;
    uint8_t t6 = fwrite(p->sizes, sizeof(uint64_t), p->asset_count, fh) == p->asset_count;
    uint64_t t7 = 0;
    nasset *a;
    for ( uint64_t k = 0; k < p->asset_count; k++ ) {
        a = p->assets[k];
        t7 += fwrite(&p->assets[k]->make_time, sizeof(uint64_t), 1, fh);
        t7 += fwrite(&p->assets[k]->mod_time, sizeof(uint64_t), 1, fh);
        t7 += fwrite(&p->assets[k]->mod_count, sizeof(uint64_t), 1, fh);
        t7 += fwrite(&p->assets[k]->data_len, sizeof(uint64_t), 1, fh);
        t7 += fwrite(&p->assets[k]->key_len, sizeof(uint64_t), 1, fh);
        t7 += fwrite(p->assets[k]->key, sizeof(wchar_t), p->assets[k]->key_len, fh);
        t7 += fwrite(p->assets[k]->data, sizeof(char), p->assets[k]->data_len, fh);
        t7 = t7 == (5 + p->assets[k]->key_len + p->assets[k]->data_len);
        if ( t7 != 1 )
            break;
    }
    fclose(fh);
    if ( t1 && t2 && t3 && t4 && t5 && t6 && t7 && t8 && t9 && t10 && t11 )
        return 1;
    return 0;
}

uint8_t npackage_close(npackage *p)
{
    for ( uint64_t q = 0; q < p->asset_count; q++ )
    {
        nasset *a = p->assets[q];
        if ( a->key != NULL )
            free(a->key);
        if ( a->data != NULL )
            free(a->data);
        free(a);
    }
    if ( p->sizes != NULL )
        free(p->sizes);
    if ( p->assets != NULL )
        free(p->assets);
    free(p);
    return 1;
}

uint8_t npackage_compression(npackage *p)
{
    return p->compression;
}

uint8_t npackage_encrypted(npackage *p)
{
    return p->encryption;
}

uint8_t nasset_close(nasset *a)
{
    if ( a == NULL )
        return 0;
    if ( a->key != NULL ) {
        free(a->key);
        a->key = NULL;
    }
    if ( a->data != NULL ) {
        free(a->data);
        a->data = NULL;
    }
    free(a);
    return 1;
}

int8_t nasset_cmp(nasset *p, nasset *q)
{
    int8_t t1 = p->make_time > q->make_time ? 1 : ( p->make_time < q->make_time ? -1 : 0 );
    int8_t t2 = p->mod_time > q->mod_time ? 1 : ( p->mod_time < q->mod_time ? -1 : 0 );
    int8_t t3 = p->mod_count > q->mod_count ? 1 : ( p->mod_count < q->mod_count ? -1 : 0 );
    int8_t t4 = p->data_len > q->data_len ? 1 : ( p->data_len < q->data_len ? -1 : 0 );
    int8_t t5 = p->key_len > q->key_len ? 1 : ( p->key_len < q->key_len ? -1 : 0 );
    if ( t1 != 0 )
        return t1;
    if ( t2 != 0 )
        return t2;
    if ( t3 != 0 )
        return t3;
    if ( t4 != 0 )
        return t4;
    if ( t5 != 0 )
        return t5;
    t1 = arrcmp((uint8_t*)p->key, (uint8_t*)q->key, p->key_len);
    if ( t1 != 0 )
        return t1;
    t1 = arrcmp((uint8_t*)p->data, (uint8_t*)q->data, p->data_len);
    return t1;
}

uint8_t nasset_delete(npackage *p, const wchar_t *k)
{
    nasset *a = nasset_unset(p, k);
    return nasset_close(a);
}

nasset * nasset_from_local(const wchar_t *k, const wchar_t *fp)
{
    nasset *a = new_nasset();
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
        if ( wcscmp(p->assets[q]->key, k) == 0 )
            return p->assets[q];
    }
    return NULL;
}

nasset *new_nasset(void)
{
    nasset *a = (nasset*)malloc(sizeof(nasset));
    a->package = NULL;
    a->make_time = time(NULL);
    a->mod_time = 0;
    a->mod_count = 0;
    a->data_len = 0;
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

    /* dealloc */
    if ( a->data != NULL )
        free(a->data);

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
        s += nasset_size(p->assets[k]);
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

    nasset **pa;
    if ( p->assets != NULL )
        pa = (nasset**)realloc(p->assets, p->asset_count * sizeof(nasset));
    else
        pa = (nasset**)malloc(sizeof(nasset));
    if ( pa == NULL )
        return 0;
    p->assets = pa;
    p->assets[p->asset_count-1] = a;

    p->mod_time = time(NULL);
    p->mod_count++;
    a->package = p;
    return 1;
}

nasset* nasset_unset(npackage *p, const wchar_t *k)
{
    for ( uint64_t q = 0; q < p->asset_count; q++ )
    {
        if ( wcscmp(p->assets[q]->key, k) == 0 )
        {
            nasset *a = p->assets[q];
            uint64_t ac = p->asset_count;
            uint64_t k = 0;

            p->mod_count++;
            p->mod_time = time(NULL);

            p->asset_count--;
            uint64_t* sz = (uint64_t*)malloc(p->asset_count * sizeof(uint64_t));
            nasset** pa = (nasset**)malloc(p->asset_count * sizeof(nasset));

            /*
            reassign, skip over q
            */
            for (; k < q; k++ )
            {
                sz[k] = p->sizes[k];
                pa[k] = p->assets[k];
            }
            k++;
            for (; k < ac; k++ )
            {
                sz[k-1] = p->sizes[k];
                pa[k-1] = p->assets[k];
            }
            free(p->sizes);
            free(p->assets);
            p->sizes = sz;
            p->assets = pa;
            return a;
    }}
    return NULL;
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
        s += nasset_size(p->assets[k]);
    return s;
}
