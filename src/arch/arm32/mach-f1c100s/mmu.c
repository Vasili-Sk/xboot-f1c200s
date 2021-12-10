/*
 * mmu.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <cache.h>

enum {
	MAP_TYPE_NCNB	= 0x0,
	MAP_TYPE_NCB	= 0x1,
	MAP_TYPE_CNB	= 0x2,
	MAP_TYPE_CB		= 0x3,
};

static uint32_t __mmu_ttb[4096] __attribute__((aligned(0x4000)));

static inline void mmu_ttb_set(uint32_t base)
{
	__asm__ __volatile__("mcr p15, 0, %0, c2, c0, 0" : : "r" (base) : "memory");
}

static inline void mmu_domain_set(uint32_t domain)
{
	__asm__ __volatile__("mcr p15, 0, %0, c3, c0, 0" : : "r" (domain) : "memory");
}

static inline void mmu_inv_tlb(void)
{
	__asm__ __volatile__("mcr p15, 0, %0, c8, c7, 0" : : "r" (0));
	__asm__ __volatile__("mcr p15, 0, %0, c8, c6, 0" : : "r" (0));
	__asm__ __volatile__("mcr p15, 0, %0, c8, c5, 0" : : "r" (0));
	dsb();
	isb();
}

static void map_l1_section(virtual_addr_t virt, physical_addr_t phys, physical_size_t size, int type)
{
	physical_size_t i;

	virt >>= 20;
	phys >>= 20;
	size >>= 20;
	type &= 0x3;

	for(i = size; i > 0; i--, virt++, phys++)
		__mmu_ttb[virt] = (phys << 20) | (1 << 16) | (0x3 << 10) | (0x0 << 5) | (type << 2) | (0x2 << 0);
}

void mmu_setup(void)
{
	extern unsigned char __dma_start[];
	extern unsigned char __dma_end[];

	map_l1_section(0x00000000, 0x00000000, SZ_2G, 0);
	map_l1_section(0x80000000, 0x80000000, SZ_2G, 0);
	map_l1_section(0x80000000, 0x80000000, SZ_32M, MAP_TYPE_CB);
	map_l1_section((virtual_addr_t)__dma_start, (physical_addr_t)__dma_start, (physical_size_t)(__dma_end - __dma_start), MAP_TYPE_NCNB);
}

void mmu_enable(void)
{
	mmu_ttb_set((uint32_t)(__mmu_ttb));
	cache_inv_range(0, ~0);
	outer_cache_enable();
	outer_cache_inv_range(0, ~0);
	mmu_inv_tlb();
	mmu_domain_set(0x3);
	arm32_mmu_enable();
	arm32_icache_enable();
	arm32_dcache_enable();
}
