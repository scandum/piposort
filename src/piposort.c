/*
	Copyright (C) 2014-2022 Igor van den Hoven ivdhoven@gmail.com
*/

/*
	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
	piposort 1.1.5.4
*/

void FUNC(branchless_oddeven_sort)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	VAR swap, *pta, *pte;
	unsigned char w, x, y, z = 1;

	switch (nmemb)
	{
		default:
			pte = array + nmemb - 3;
			do
			{
				w = z;
				pta = pte + (z = !z);

				do
				{
					x = cmp(pta, pta + 1) > 0; y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta -= 2; w |= x;
				}
				while (pta >= array);
			}
			while (w && --nmemb);
			return;
		case 3:
			pta = array;
			x = cmp(pta, pta + 1) > 0; y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap; pta++;
			x = cmp(pta, pta + 1) > 0; y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap;
		case 2:
			pta = array;
			x = cmp(pta, pta + 1) > 0; y = !x; swap = pta[y]; pta[0] = pta[x]; pta[1] = swap;
		case 1:
		case 0:
			return;
	}
}

void FUNC(oddeven_parity_merge)(VAR *from, VAR *dest, size_t left, size_t right, CMPFUNC *cmp)
{
	VAR *ptl, *ptr, *tpl, *tpr, *tpd, *ptd;
	unsigned char x;

	ptl = from; ptr = from + left; ptd = dest;
	tpl = from + left - 1; tpr = from + left + right - 1; tpd = dest + left + right - 1;

	if (left < right)
	{
		*ptd++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	}

	while (--left)
	{
		x = cmp(ptl, ptr) <= 0; *ptd = *ptl; ptl += x; ptd[x] = *ptr; ptr += !x; ptd++;
		x = cmp(tpl, tpr) <= 0; *tpd = *tpl; tpl -= !x; tpd--; tpd[x] = *tpr; tpr -= x;
	}
	*tpd = cmp(tpl, tpr)  > 0 ? *tpl : *tpr;
	*ptd = cmp(ptl, ptr) <= 0 ? *ptl : *ptr;
}

void FUNC(auxiliary_rotation)(VAR *array, VAR *swap, size_t left, size_t right)
{
	memcpy(swap, array, left * sizeof(VAR));

	memmove(array, array + left, right * sizeof(VAR));

	memcpy(array + right, swap, left * sizeof(VAR));
}

void FUNC(ping_pong_merge)(VAR *array, VAR *swap, size_t nmemb, CMPFUNC *cmp)
{
	size_t quad1, quad2, quad3, quad4, half1, half2;

	if (nmemb <= 7)
	{
		FUNC(branchless_oddeven_sort)(array, nmemb, cmp);
		return;
	}
	half1 = nmemb / 2;
	quad1 = half1 / 2;
	quad2 = half1 - quad1;
	half2 = nmemb - half1;
	quad3 = half2 / 2;
	quad4 = half2 - quad3;

	FUNC(ping_pong_merge)(array, swap, quad1, cmp);
	FUNC(ping_pong_merge)(array + quad1, swap, quad2, cmp);
	FUNC(ping_pong_merge)(array + half1, swap, quad3, cmp);
	FUNC(ping_pong_merge)(array + half1 + quad3, swap, quad4, cmp);

	if (cmp(array + quad1 - 1, array + quad1) <= 0 && cmp(array + half1 - 1, array + half1) <= 0 && cmp(array + half1 + quad3 - 1, array + half1 + quad3) <= 0)
	{
		return;
	}

	if (cmp(array, array + half1 - 1) > 0 && cmp(array + quad1, array + half1 + quad3 - 1) > 0 && cmp(array + half1, array + nmemb - 1) > 0)
	{
		FUNC(auxiliary_rotation)(array, swap, quad1, quad2 + half2);
		FUNC(auxiliary_rotation)(array, swap, quad2, half2);
		FUNC(auxiliary_rotation)(array, swap, quad3, quad4);
		return;
	}

	FUNC(oddeven_parity_merge)(array, swap, quad1, quad2, cmp);
	FUNC(oddeven_parity_merge)(array + half1, swap + half1, quad3, quad4, cmp);
	FUNC(oddeven_parity_merge)(swap, array, half1, half2, cmp);
}

void FUNC(piposort)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	VAR *swap = malloc(nmemb * sizeof(VAR));

	FUNC(ping_pong_merge)(array, swap, nmemb, cmp);

	free(swap);
}
