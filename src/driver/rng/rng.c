/*
 * driver/rng/rng.c
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

#include <xboot.h>
#include <rng/rng.h>

static ssize_t rng_read_random(struct kobj_t * kobj, void * buf, size_t size)
{
	struct rng_t * rng = (struct rng_t *)kobj->priv;
	unsigned char dat = 0;

	rng_read_data(rng, &dat, 1, 0);
	return sprintf(buf, "%02x", dat);
}

struct rng_t * search_rng(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_RNG);
	if(!dev)
		return NULL;
	return (struct rng_t *)dev->priv;
}

struct rng_t * search_first_rng(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_RNG);
	if(!dev)
		return NULL;
	return (struct rng_t *)dev->priv;
}

struct device_t * register_rng(struct rng_t * rng, struct driver_t * drv)
{
	struct device_t * dev;

	if(!rng || !rng->name || !rng->read)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	dev->name = strdup(rng->name);
	dev->type = DEVICE_TYPE_RNG;
	dev->driver = drv;
	dev->priv = rng;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "random", rng_read_random, NULL, rng);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_rng(struct rng_t * rng)
{
	struct device_t * dev;

	if(rng && rng->name)
	{
		dev = search_device(rng->name, DEVICE_TYPE_RNG);
		if(dev && unregister_device(dev))
		{
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
}

int rng_read_data(struct rng_t * rng, void * buf, int max, int wait)
{
	if(rng && rng->read)
		return rng->read(rng, buf, max, wait);
	return 0;
}
