#include "context.h"
#include "context_helper.h"
#include <dlfcn.h>
#include <raylib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

updateContext_t *updateContext_p;
helloFromLib_t *helloFromLib_p;

void *libContext = NULL;
const char *libPath = "./libs/libgame.so";
const char *copyPaht = "libgame-copy.so";
time_t lastTimeEdited = 0;

void CopyFile(const char *src, const char *dest) {
  FILE *fsrc;
  FILE *fdest;
  unsigned char buffer[512];
  size_t bytes;

  fprintf(stderr, "copy from: %s to %s!\n", src, dest);

  fsrc = fopen(src, "rb");
  if (fsrc == NULL)
    fprintf(stderr, "failed to open file: %s for reading\n", src);

  fdest = fopen(dest, "wb");
  if (fdest == NULL)
    fprintf(stderr, "failed to open file: %s for reading\n", src);

  while ((bytes = fread(buffer, 1, sizeof(buffer), fsrc)) > 0) {
    fwrite(buffer, 1, bytes, fdest);
  }

  fclose(fsrc);
  fclose(fdest);

  fprintf(stderr, "copy complete!\n");
}

void *loadDynamicSymbol(void *handle, const char *symbol) {
  void *symbolPointer = dlsym(handle, symbol);
  if (symbolPointer == NULL) {
    fprintf(stdout, "ERROR: Failed to load symbol pointer: %s\n %s", symbol,
            dlerror());
    exit(1);
  }

  return symbolPointer;
}

void reloadContext() {

  if (libContext != NULL) {
    if (dlclose(libContext) != 0) {
      fprintf(stdout, "dlclose failed: %s\n", dlerror());
      exit(1);
    } else {
      libContext = NULL;
    }
  }

  libContext = dlopen(libPath, RTLD_NOW | RTLD_GLOBAL);
  if (libContext == NULL) {
    fprintf(stdout, "failed to load %s, error = %s\n", libPath, dlerror());
    exit(1);
  }
  updateContext_p =
      (updateContext_t *)loadDynamicSymbol(libContext, "updateContext");

  helloFromLib_p =
      (helloFromLib_t *)loadDynamicSymbol(libContext, "helloFromLib");
}

void updateContext(steelcpp::Context *context) { updateContext_p(context); }

int main(int argc, char *argv[]) {

  reloadContext();
  helloFromLib_p();

  struct stat s;

  steelcpp::Context game = steelcpp::Context(Vector2(800, 600), "Asteroids");
  while (game.isRunning) {

    stat(libPath, &s);

    if (s.st_mtime > lastTimeEdited) {
      lastTimeEdited = s.st_mtime;
      printf("Last Time Edited: %ld\n", lastTimeEdited);
      reloadContext();
    }

    if (IsKeyPressed(KEY_R))
      reloadContext();
    updateContext(&game);
  }

  return 0;
}
