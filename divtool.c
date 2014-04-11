#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEF_CHUNK 1024*1024*256

static char *ProgName;

void usage()
{
  fprintf(stderr, "Usage: %s [-c chunk] filename\n", ProgName);
  fprintf(stderr, "Options:\n"
                  "  -c chunk   specify the size of one file (default: %d)\n"
                  "  -h         show this message\n"
          , DEF_CHUNK);
}

#define BUFSIZE 4096

int main(int argc, char **argv)
{
  size_t chunk = DEF_CHUNK;
  char buff[BUFSIZE];
  int ch;
  char *fname;
  FILE *fp;

  ProgName = argv[0];

  while ((ch = getopt(argc, argv, "c:h")) != -1) {
    switch (ch) {
    case 'c':
      chunk = atoi(optarg);
      break;
    case 'h':
      usage();
      exit(EXIT_SUCCESS);
    default:
      usage();
      exit(EXIT_FAILURE);
    }
  }
  argc -= optind;
  argv += optind;

  /* fprintf(stderr, "DEBUG: argc=%d\n", argc); */
  if (argc != 1) {
    fprintf(stderr, "ERROR: filename not specified\n");
    usage();
    exit(EXIT_FAILURE);
  }
  /* fprintf(stderr, "DEBUG: argv[0]=%s\n", argv[0]); */
  fname = argv[0];

  fp = fopen(fname, "r");
  if (fp == NULL) {
    fprintf(stderr, "ERROR: cannot open file %s\n", fname);
    exit(EXIT_FAILURE);
  }

  int done = 0;
  int count;
  size_t tsum = 0;
  for (count = 0; ; count++) {
    char subfname[256];
    FILE *subfp;
    size_t subcount = (chunk + BUFSIZE - 1) / BUFSIZE;
    size_t size = BUFSIZE;
    size_t sum = 0;
    int i;

    int ret = snprintf(subfname, 256, "%s.%d", fname, count);
    if (ret >= 256) {
      fprintf(stderr, "ERROR: filename too long\n");
      exit(EXIT_FAILURE);
    }
    subfp = fopen(subfname, "w");
    if (subfp == NULL) {
      fprintf(stderr, "ERROR: cannot open file %s\n", subfname);
      exit(EXIT_FAILURE);
    }
    for (i = 0; i < subcount; i++) {
      if (i == subcount - 1) size = chunk % BUFSIZE;
      ret = fread(buff, sizeof(char), size, fp);
      /* fprintf(stderr, "DEBUG: %d bytes read\n", size); */
      if (ret < size) {
	size = ret;
	done = 1;
      }
      ret = fwrite(buff, sizeof(char), size, subfp);
      if (ret != size) {
	fprintf(stderr, "ERROR: write failed\n");
	exit(EXIT_FAILURE);
      }
      /* fprintf(stderr, "DEBUG: %d bytes written\n", ret); */
      sum += ret;
      if (done) break;
    }
    fclose(subfp);
    fprintf(stderr, "INFO: wrote %s (%ld bytes)\n", subfname, sum);
    tsum += sum;
    if (done) break;
  }
  fprintf(stderr, "INFO: total %ld bytes\n", tsum);
  fclose(fp);
  exit(EXIT_SUCCESS);
}
