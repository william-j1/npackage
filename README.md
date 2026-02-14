NPackage
========

Package management standard for assets and datasets.

![snapshot](docs/scr.png)

Objective
=========

Safe wrapper for the distribution of assets enclosed in multimedia projects. This includes bitmaps, audio, numeric and geometic datasets distributed with large projects such as cloud enabled platform data exchanges and video game engine pipelines for complex asset types. Simplified layer for the importation and exportation of distinct data types.

Usage
=====

1. Add `npackage.h`, `npackage.c`, `utils.h`, `utils.c` to your project.
2. Include the header:
   ```
   #include "npackage.h"
   ```
3. Use the provided functions to load and save npackages, set and unset nassets

Use Case Sample
===============

```
#include "npackage.h"

int main(void)
{
  npackage *p = new_npackage();
  nasset *a = new_nasset();
  wchar_t *k = (wchar_t *)malloc(6 * sizeof(wchar_t));
  wcsncpy(k, L"uid_n", 6);
  k1[5] = L'\0';
  nasset_set_key(a, k);
  nasset_set_value(a, d, data_len);
  nasset_insert(p, a);
  npackage_save(fp, p);
  return 0;
}
```

... replacing `d`, `data_len` and `fp` with a unsigned byte array, array size and file path respectively.

License
=======

This project is licensed under the LGPL v2.1
