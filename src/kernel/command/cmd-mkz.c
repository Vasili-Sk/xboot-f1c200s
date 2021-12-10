/*
 * kernel/command/cmd-mkz.c
 */

#include <xboot.h>
#include <lz4.h>
#include <lz4hc.h>
#include <sha256.h>
#include <ecdsa256.h>
#include <command/command.h>

struct zdesc_t {			/* Total 256 bytes */
	uint8_t magic[4];		/* ZBL! */
	uint8_t signature[64];	/* Ecdsa256 signature of sha256 */
	uint8_t sha256[32];		/* Sha256 hash */
	uint8_t majoy;			/* Majoy version */
	uint8_t minior;			/* Minior version */
	uint8_t patch;			/* Patch version */
	uint8_t csize[4];		/* Compress size of image */
	uint8_t dsize[4];		/* Decompress size of image */
	uint8_t pubkey[33];		/* Ecdsa256 public key */
	uint8_t message[112];	/* Message additionally */
};

static void usage(void)
{
	printf("usage:\r\n");
	printf("    mkz [-r reserve-image-size] [-pb ecdsa256-public-key] [-pv ecdsa256-private-key] [-m message] <image> <zimage>\r\n");
	printf("    -r      The reserve size\r\n");
	printf("    -pb     The ecdsa256 public key\r\n");
	printf("    -pv     The ecdsa256 private key\r\n");
	printf("    -m      The additional message\r\n");
}

static inline unsigned char hex_to_bin(char c)
{
	if((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	if((c >= '0') && (c <= '9'))
		return c - '0';
	if((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	return 0;
}

static inline unsigned char hex_string(const char * s, int o)
{
	return (hex_to_bin(s[o]) << 4) | hex_to_bin(s[o + 1]);
}

static int do_mkz(int argc, char ** argv)
{
	struct sha256_ctx_t shactx;
	struct vfs_stat_t st;
	struct zdesc_t * z;
	char blpath[VFS_MAX_PATH] = { 0 };
	char zblpath[VFS_MAX_PATH] = { 0 };
	char * blbuf = NULL;
	char * zblbuf = NULL;
	char * msg = NULL;
	uint8_t public[33] = {
		0x03, 0xcf, 0xd1, 0x8e, 0x4a, 0x4b, 0x40, 0xd6,
		0x52, 0x94, 0x48, 0xaa, 0x2d, 0xf8, 0xbb, 0xb6,
		0x77, 0x12, 0x82, 0x58, 0xb8, 0xfb, 0xfc, 0x5b,
		0x9e, 0x49, 0x2f, 0xbb, 0xba, 0x4e, 0x84, 0x83,
		0x2f,
	};
	uint8_t private[32] = {
		0xdc, 0x57, 0xb8, 0xa9, 0xe0, 0xe2, 0xb7, 0xf8,
		0xb4, 0xc9, 0x29, 0xbd, 0x8d, 0xb2, 0x84, 0x4e,
		0x53, 0xf0, 0x1f, 0x17, 0x1b, 0xbc, 0xdf, 0x6e,
		0x62, 0x89, 0x08, 0xdb, 0xf2, 0xb2, 0xe6, 0xa9,
	};
	int index = 0, rsize = 0;
	int fd, bllen, zbllen;
	int clen, len;
	int i, o;

	if(argc < 2)
	{
		usage();
		return -1;
	}
	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-r") && (argc > i + 1))
		{
			rsize = (int)strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "-pb") && (argc > i + 1))
		{
			char * p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) == 33 * 2))
			{
				for(o = 0; o < 33; o++)
					public[o] = hex_string(p, o * 2);
			}
			i++;
		}
		else if(!strcmp(argv[i], "-pv") && (argc > i + 1))
		{
			char * p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) == 32 * 2))
			{
				for(o = 0; o < 32; o++)
					private[o] = hex_string(p, o * 2);
			}
			i++;
		}
		else if(!strcmp(argv[i], "-m") && (argc > i + 1))
		{
			char * p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) > 0))
				msg = p;
			i++;
		}
		else if(*argv[i] == '-')
		{
			usage();
			return -1;
		}
		else if(*argv[i] != '-' && strcmp(argv[i], "-") != 0)
		{
			if(index == 0)
			{
				if(shell_realpath(argv[i], blpath) < 0)
				{
					usage();
					return -1;
				}
			}
			else if(index == 1)
			{
				if(shell_realpath(argv[i], zblpath) < 0)
				{
					usage();
					return -1;
				}
			}
			else
			{
				usage();
				return -1;
			}
			index++;
		}
	}
	if((strcmp(blpath, "") == 0) || (strlen(blpath) <= 0) || (strcmp(zblpath, "") == 0) || (strlen(zblpath) <= 0))
	{
		usage();
		return -1;
	}
	if((vfs_stat(blpath, &st) < 0) || !S_ISREG(st.st_mode))
	{
		printf("Can not stat image\r\n");
		return -1;
	}
	bllen = st.st_size;
	if(rsize > bllen)
	{
		printf("The reserve size is too large\r\n");
		return -1;
	}
	fd = vfs_open(blpath, O_RDONLY, 0);
	if(fd < 0)
	{
		printf("Open image error\r\n");
		return -1;
	}
	blbuf = malloc(bllen);
	memset(blbuf, 0, bllen);
	if(vfs_read(fd, blbuf, bllen) != bllen)
	{
		printf("Can't read image\r\n");
		free(blbuf);
		vfs_close(fd);
		return -1;
	}
	vfs_close(fd);

	len = LZ4_compressBound(bllen);
	zbllen = rsize + sizeof(struct zdesc_t) + len;
	zblbuf = malloc(zbllen);
	memset(zblbuf, 0, zbllen);
	memcpy(&zblbuf[0], &blbuf[0], rsize);
	clen = LZ4_compress_HC(&blbuf[0], &zblbuf[rsize + sizeof(struct zdesc_t)], bllen, len, 12);
	zbllen = rsize + sizeof(struct zdesc_t) + clen;

	z = (struct zdesc_t *)&zblbuf[rsize];
	z->magic[0] = 'Z';
	z->magic[1] = 'B';
	z->magic[2] = 'L';
	z->magic[3] = '!';
	z->majoy = xboot_version() / 100;
	z->minior = (xboot_version() % 100) / 10;
	z->patch = xboot_version() % 10;
	z->csize[0] = (clen >> 24) & 0xff;
	z->csize[1] = (clen >> 16) & 0xff;
	z->csize[2] = (clen >>  8) & 0xff;
	z->csize[3] = (clen >>  0) & 0xff;
	z->dsize[0] = (bllen >> 24) & 0xff;
	z->dsize[1] = (bllen >> 16) & 0xff;
	z->dsize[2] = (bllen >>  8) & 0xff;
	z->dsize[3] = (bllen >>  0) & 0xff;
	memcpy(&z->pubkey[0], &public[0], 33);
	if(msg)
		strlcpy((char *)&z->message[0], msg, 112);

	sha256_init(&shactx);
	sha256_update(&shactx, (void *)(&z->majoy), 1);
	sha256_update(&shactx, (void *)(&z->minior), 1);
	sha256_update(&shactx, (void *)(&z->patch), 1);
	sha256_update(&shactx, (void *)(&z->csize[0]), 4);
	sha256_update(&shactx, (void *)(&z->dsize[0]), 4);
	sha256_update(&shactx, (void *)(&z->pubkey[0]), 33);
	sha256_update(&shactx, (void *)(&z->message[0]), 112);
	sha256_update(&shactx, (void *)(&zblbuf[rsize + sizeof(struct zdesc_t)]), clen);
	memcpy(&z->sha256[0], sha256_final(&shactx), SHA256_DIGEST_SIZE);

	printf("Ecdsa256 public key:\r\n\t");
	for(o = 0; o < 33; o++)
		printf("%02x", z->pubkey[o]);
	printf("\r\n");
	printf("Ecdsa256 private key:\r\n\t");
	for(o = 0; o < 32; o++)
		printf("%02x", private[o]);
	printf("\r\n");

	ecdsa256_sign(private, &z->sha256[0], &z->signature[0]);
	if(!ecdsa256_verify(&z->pubkey[0], &z->sha256[0], &z->signature[0]))
	{
		printf("Ecdsa256 signature verify failed, please check the ecdsa256 public and private key!\r\n");
		free(zblbuf);
		return -1;
	}

	fd = vfs_open(zblpath, (O_WRONLY | O_CREAT | O_TRUNC), 0644);
	if(fd < 0)
	{
		printf("Open zimage error\r\n");
		free(zblbuf);
		return -1;
	}
	if(vfs_write(fd, zblbuf, zbllen) != zbllen)
	{
		printf("Write zimage error\r\n");
		free(blbuf);
		free(zblbuf);
		vfs_close(fd);
		return -1;
	}
	free(blbuf);
	free(zblbuf);
	vfs_close(fd);

	printf("Compressed %d bytes into %d bytes ==> %f%%\r\n", bllen, clen, clen * 100.0 / bllen);
	return 0;
}

static struct command_t cmd_mkz = {
	.name	= "mkz",
	.desc	= "compress and encrypt and signature image",
	.usage	= usage,
	.exec	= do_mkz,
};

static __init void mkz_cmd_init(void)
{
	register_command(&cmd_mkz);
}

static __exit void mkz_cmd_exit(void)
{
	unregister_command(&cmd_mkz);
}

command_initcall(mkz_cmd_init);
command_exitcall(mkz_cmd_exit);
