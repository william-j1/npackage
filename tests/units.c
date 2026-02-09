#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <assert.h>

#include "npackage.h"

/*
integrity unit test 1
*/
void iut1(void)
{
	npackage *p = init_npackage();
	nasset * a1 = init_nasset();
	nasset * a2 = init_nasset();
	nasset * a3 = init_nasset();
	nasset * a4 = init_nasset();
	nasset * a5 = init_nasset();

	printf("package size: %d\n", npackage_size(p));
	printf("a1 asset size: %d\n", nasset_size(a1));
	printf("a2 asset size: %d\n", nasset_size(a2));
	printf("a3 asset size: %d\n", nasset_size(a3));
	printf("a4 asset size: %d\n", nasset_size(a4));
	printf("a5 asset size: %d\n", nasset_size(a5));

	size_t l = 50;
	wchar_t *k1 = (wchar_t *)malloc(l * sizeof(wchar_t));
	wchar_t *k2 = (wchar_t *)malloc(l * sizeof(wchar_t));
	wchar_t *k3 = (wchar_t *)malloc(l * sizeof(wchar_t));
	wchar_t *k4 = (wchar_t *)malloc(l * sizeof(wchar_t));
	wchar_t *k5 = (wchar_t *)malloc(l * sizeof(wchar_t));

	wcsncpy(k1, L"key1", l);
	wcsncpy(k2, L"key22", l);
	wcsncpy(k3, L"key333", l);
	wcsncpy(k4, L"key4444", l);
	wcsncpy(k5, L"key55555", l);
    k1[l - 1] = L'\0';
	k2[l - 1] = L'\0';
	k3[l - 1] = L'\0';
	k4[l - 1] = L'\0';
	k5[l - 1] = L'\0';
    wprintf(L"0 : %ls\n", k1);
	wprintf(L"1 : %ls\n", k2);
	wprintf(L"2 : %ls\n", k3);
	wprintf(L"3 : %ls\n", k4);
	wprintf(L"4 : %ls\n", k5);
	unsigned char *d1 = (unsigned char *)malloc(1 * sizeof(unsigned char));
	unsigned char *d2 = (unsigned char *)malloc(2 * sizeof(unsigned char));
	unsigned char *d3 = (unsigned char *)malloc(3 * sizeof(unsigned char));
	unsigned char *d4 = (unsigned char *)malloc(4 * sizeof(unsigned char));
	unsigned char *d5 = (unsigned char *)malloc(5 * sizeof(unsigned char));

	d1[0] = 0;
	d2[0] = 1;
	d2[1] = 1;
	d3[0] = 2;
	d3[1] = 2;
	d3[2] = 2;
	d4[0] = 3;
	d4[1] = 3;
	d4[2] = 3;
	d4[3] = 3;
	d5[0] = 4;
	d5[1] = 4;
	d5[2] = 4;
	d5[3] = 4;
	d5[4] = 4;

	nasset_set_key(a1, k1);
	nasset_set_value(a1, d1, 1);
	nasset_insert(p, a1);
	printf("package size: %d\n", npackage_size(p));
	printf("a1 asset size: %d\n", nasset_size(a1));

	nasset_set_key(a2, k2);
	nasset_set_value(a2, d2, 2);
	nasset_insert(p, a2);
	printf("package size: %d\n", npackage_size(p));
	printf("a2 asset size: %d\n", nasset_size(a2));

	nasset_set_key(a3, k3);
	nasset_set_value(a3, d3, 3);
	nasset_insert(p, a3);
	printf("package size: %d\n", npackage_size(p));
	printf("a3 asset size: %d\n", nasset_size(a3));

	nasset_set_key(a4, k4);
	nasset_set_value(a4, d4, 4);
	nasset_insert(p, a4);
	printf("package size: %d\n", npackage_size(p));
	printf("a4 asset size: %d\n", nasset_size(a4));

	nasset_set_key(a5, k5);
	nasset_set_value(a5, d5, 5);
	nasset_insert(p, a5);
	printf("package size: %d\n", npackage_size(p));
	printf("a5 asset size: %d\n", nasset_size(a5));

	printf("header size %d\n", npackage_header_size(p));
	printf("asset sizes: %d\n", nassets_size(p));

	assert(npackage_size(p) == npackage_header_size(p) + nassets_size(p));
	free(d1);
	free(d2);
	free(d3);
	free(d4);
	free(d5);
    free(k1);
	free(k2);
	free(k3);
	free(k4);
	free(k5);
	free(a1);
	free(a2);
	free(a3);
	free(a4);
	free(a5);
	free(p);
}

/*
integrity unit test 2
*/
void iut2(void)
{
	npackage *p = init_npackage();
	nasset *a1 = init_nasset();

	wchar_t *k1 = (wchar_t *)malloc(5 * sizeof(wchar_t));
	wchar_t *k2 = (wchar_t *)malloc(5 * sizeof(wchar_t));
	wcsncpy(k1, L"key1", 4);
    k1[4] = L'\0';
	wcsncpy(k2, L"key1", 4);
    k2[4] = L'\0';
	unsigned char *d1 = (unsigned char *)malloc(4 * sizeof(unsigned char));
	d1[0] = 3;
	d1[1] = 1;
	d1[2] = 2;
	d1[3] = 0;

	nasset_set_key(a1, k1);
	nasset_set_value(a1, d1, 4);
	/*
	mods only tracked after first set
	*/
	assert(a1->mod_count == 0);

	nasset_insert(p, a1);
	assert(npackage_has_key(p, k2) != 0);
	nasset *a2 = nasset_get(p, k2);
	assert(a2->data[0] == 3);
	assert(a2->data[1] == 1);
	assert(a2->data[2] == 2);
	assert(a2->data[3] == 0);
	assert(p->mod_count == 1);
	free(a1);
	free(d1);
	free(k1);
	free(k2);
	free(p);
}

/*
integrity unit test 3
*/
void iut3()
{
	wchar_t *k1 = (wchar_t *)malloc(4 * sizeof(wchar_t));
	wchar_t *fp1 = (wchar_t *)malloc(13 * sizeof(wchar_t));
	wcsncpy(k1, L"key", 3);
	wcsncpy(fp1, L"tests\\iut3_1", 13);
	k1[3] = L'\0';
    fp1[12] = L'\0';
	nasset *a = nasset_from_disk(k1, fp1);
	printf("%d bytes\n", a->data_len);
	wprintf(L"0 => %d\n", a->data[0]);
	wprintf(L"1 => %d\n", a->data[1]);
	wprintf(L"2 => %d\n", a->data[2]);
	wprintf(L"3 => %d\n", a->data[3]);
	wprintf(L"4 => %d\n", a->data[4]);
	wprintf(L"5 => %d\n", a->data[5]);
	wprintf(L"6 => %d\n", a->data[6]);
	assert(a->data_len == 7);
	assert(a->data[0] == 55);
	assert(a->data[6] == 115);
	free(a);
	free(fp1);
	free(k1);
}

/*
integrity unit test 4
*/
void iut4()
{
	wchar_t *fp = (wchar_t *)malloc(13 * sizeof(wchar_t));
	wcsncpy(fp, L"tests\\iut4_1", 13);
    fp[12] = L'\0';
	npackage *p = init_npackage();
	nasset * a1 = init_nasset();
	wchar_t *k1 = (wchar_t *)malloc(5 * sizeof(wchar_t));
	wcsncpy(k1, L"key1", 4);
    k1[4] = L'\0';
    wprintf(L"0 : %ls\n", k1);
	unsigned char *d1 = (unsigned char *)malloc(1 * sizeof(unsigned char));
	d1[0] = 184;
	nasset_set_key(a1, k1);
	nasset_set_value(a1, d1, 1);
	nasset_insert(p, a1);
	printf("export sum: %d\n", npackage_save(fp, p));

    npackage *q = npackage_load(fp);
    if ( q != NULL )
        printf("asset count: %d\n", q->asset_count);
    else
        printf("load failed");
    assert(q->assets[0].data[0] == d1[0]);

	free(fp);
	free(k1);
	free(d1);
	free(a1);
	free(p);
}
