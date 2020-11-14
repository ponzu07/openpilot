#include "common/params.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif  // _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/file.h>
#include <sys/stat.h>

#include <map>
#include <string>
<<<<<<< HEAD
=======
#include <iostream>
#include <csignal>
>>>>>>> origin/ci-clean
#include <string.h>

#include "common/util.h"
#include "common/utilpp.h"


<<<<<<< HEAD
namespace {

template <typename T>
T* null_coalesce(T* a, T* b) {
  return a != NULL ? a : b;
}

static const char* default_params_path = null_coalesce(const_cast<const char*>(getenv("PARAMS_PATH")), "/data/params");

#ifdef QCOM
static const char* persistent_params_path = null_coalesce(const_cast<const char*>(getenv("PERSISTENT_PARAMS_PATH")), "/persist/comma/params");
#else
static const char* persistent_params_path = default_params_path;
#endif

} //namespace


static int fsync_dir(const char* path){
  int result = 0;
  int fd = open(path, O_RDONLY);

  if (fd < 0){
    result = -1;
    goto cleanup;
  }

  result = fsync(fd);
  if (result < 0) {
    goto cleanup;
  }

 cleanup:
  int result_close = 0;
  if (fd >= 0){
    result_close = close(fd);
  }

  if (result_close < 0) {
    return result_close;
  } else {
    return result;
  }
}

static int ensure_dir_exists(const char* path) {
  struct stat st;
  if (stat(path, &st) == -1) {
    return mkdir(path, 0700);
  }
  return 0;
}

int write_db_value(const char* key, const char* value, size_t value_size, bool persistent_param) {
=======
std::string getenv_default(const char* env_var, const char * suffix, const char* default_val) {
  const char* env_val = getenv(env_var);
  if (env_val != NULL){
    return std::string(env_val) + std::string(suffix);
  } else{
    return std::string(default_val);
  }
}

#if defined(QCOM) || defined(QCOM2)
const std::string default_params_path = "/data/params";
#else
const std::string default_params_path = getenv_default("HOME", "/.comma/params", "/data/params");
#endif

#if defined(QCOM) || defined(QCOM2)
const std::string persistent_params_path = "/persist/comma/params";
#else
const std::string persistent_params_path = default_params_path;
#endif


volatile sig_atomic_t params_do_exit = 0;
void params_sig_handler(int signal) {
  params_do_exit = 1;
}

static int fsync_dir(const char* path){
  int fd = open(path, O_RDONLY, 0755);
  if (fd < 0){
    return -1;
  }

  int result = fsync(fd);
  int result_close = close(fd);
  if (result_close < 0) {
    result = result_close;
  }
  return result;
}

static int mkdir_p(std::string path) {
  char * _path = (char *)path.c_str();

  mode_t prev_mask = umask(0);
  for (char *p = _path + 1; *p; p++) {
    if (*p == '/') {
      *p = '\0'; // Temporarily truncate
      if (mkdir(_path, 0777) != 0) {
        if (errno != EEXIST) return -1;
      }
      *p = '/';
    }
  }
  if (mkdir(_path, 0777) != 0) {
    if (errno != EEXIST) return -1;
  }
  chmod(_path, 0777);
  umask(prev_mask);
  return 0;
}

static int ensure_dir_exists(std::string path) {
  // TODO: replace by std::filesystem::create_directories
  return mkdir_p(path.c_str());
}


Params::Params(bool persistent_param){
  params_path = persistent_param ? persistent_params_path : default_params_path;
}

Params::Params(std::string path) {
  params_path = path;
}

int Params::write_db_value(std::string key, std::string dat){
  return write_db_value(key.c_str(), dat.c_str(), dat.length());
}

int Params::write_db_value(const char* key, const char* value, size_t value_size) {
>>>>>>> origin/ci-clean
  // Information about safely and atomically writing a file: https://lwn.net/Articles/457667/
  // 1) Create temp file
  // 2) Write data to temp file
  // 3) fsync() the temp file
  // 4) rename the temp file to the real name
  // 5) fsync() the containing directory

  int lock_fd = -1;
  int tmp_fd = -1;
  int result;
<<<<<<< HEAD
  char tmp_path[1024];
  char path[1024];
  char *tmp_dir;
  ssize_t bytes_written;
  const char* params_path = persistent_param ? persistent_params_path : default_params_path;
=======
  std::string path;
  std::string tmp_path;
  ssize_t bytes_written;
>>>>>>> origin/ci-clean

  // Make sure params path exists
  result = ensure_dir_exists(params_path);
  if (result < 0) {
    goto cleanup;
  }

<<<<<<< HEAD
  result = snprintf(path, sizeof(path), "%s/d", params_path);
  if (result < 0) {
    goto cleanup;
  }

  // See if the symlink exists, otherwise create it
  struct stat st;
  if (stat(path, &st) == -1) {
    // Create temp folder
    result = snprintf(path, sizeof(path), "%s/.tmp_XXXXXX", params_path);
    if (result < 0) {
      goto cleanup;
    }
    tmp_dir = mkdtemp(path);
    if (tmp_dir == NULL){
      goto cleanup;
    }

    // Set permissions
    result = chmod(tmp_dir, 0777);
=======
  // See if the symlink exists, otherwise create it
  path = params_path + "/d";
  struct stat st;
  if (stat(path.c_str(), &st) == -1) {
    // Create temp folder
    path = params_path + "/.tmp_XXXXXX";

    char *t = mkdtemp((char*)path.c_str());
    if (t == NULL){
      goto cleanup;
    }
    std::string tmp_dir(t);

    // Set permissions
    result = chmod(tmp_dir.c_str(), 0777);
>>>>>>> origin/ci-clean
    if (result < 0) {
      goto cleanup;
    }

    // Symlink it to temp link
<<<<<<< HEAD
    result = snprintf(tmp_path, sizeof(tmp_path), "%s.link", tmp_dir);
    if (result < 0) {
      goto cleanup;
    }
    result = symlink(tmp_dir, tmp_path);
=======
    tmp_path = tmp_dir + ".link";
    result = symlink(tmp_dir.c_str(), tmp_path.c_str());
>>>>>>> origin/ci-clean
    if (result < 0) {
      goto cleanup;
    }

    // Move symlink to <params>/d
<<<<<<< HEAD
    result = snprintf(path, sizeof(path), "%s/d", params_path);
    if (result < 0) {
      goto cleanup;
    }
    result = rename(tmp_path, path);
=======
    path = params_path + "/d";
    result = rename(tmp_path.c_str(), path.c_str());
    if (result < 0) {
      goto cleanup;
    }
  } else {
    // Ensure permissions are correct in case we didn't create the symlink
    result = chmod(path.c_str(), 0777);
>>>>>>> origin/ci-clean
    if (result < 0) {
      goto cleanup;
    }
  }

  // Write value to temp.
<<<<<<< HEAD
  result =
      snprintf(tmp_path, sizeof(tmp_path), "%s/.tmp_value_XXXXXX", params_path);
  if (result < 0) {
    goto cleanup;
  }

  tmp_fd = mkstemp(tmp_path);
  bytes_written = write(tmp_fd, value, value_size);
  if (bytes_written != value_size) {
=======
  tmp_path = params_path + "/.tmp_value_XXXXXX";
  tmp_fd = mkstemp((char*)tmp_path.c_str());
  bytes_written = write(tmp_fd, value, value_size);
  if (bytes_written < 0 || (size_t)bytes_written != value_size) {
>>>>>>> origin/ci-clean
    result = -20;
    goto cleanup;
  }

  // Build lock path
<<<<<<< HEAD
  result = snprintf(path, sizeof(path), "%s/.lock", params_path);
  if (result < 0) {
    goto cleanup;
  }
  lock_fd = open(path, O_CREAT);

  // Build key path
  result = snprintf(path, sizeof(path), "%s/d/%s", params_path, key);
  if (result < 0) {
    goto cleanup;
  }
=======
  path = params_path + "/.lock";
  lock_fd = open(path.c_str(), O_CREAT, 0775);

  // Build key path
  path = params_path + "/d/" + std::string(key);
>>>>>>> origin/ci-clean

  // Take lock.
  result = flock(lock_fd, LOCK_EX);
  if (result < 0) {
    goto cleanup;
  }

  // change permissions to 0666 for apks
  result = fchmod(tmp_fd, 0666);
  if (result < 0) {
    goto cleanup;
  }

  // fsync to force persist the changes.
  result = fsync(tmp_fd);
  if (result < 0) {
    goto cleanup;
  }

  // Move temp into place.
<<<<<<< HEAD
  result = rename(tmp_path, path);
=======
  result = rename(tmp_path.c_str(), path.c_str());
>>>>>>> origin/ci-clean
  if (result < 0) {
    goto cleanup;
  }

  // fsync parent directory
<<<<<<< HEAD
  result = snprintf(path, sizeof(path), "%s/d", params_path);
  if (result < 0) {
    goto cleanup;
  }

  result = fsync_dir(path);
=======
  path = params_path + "/d";
  result = fsync_dir(path.c_str());
>>>>>>> origin/ci-clean
  if (result < 0) {
    goto cleanup;
  }

cleanup:
  // Release lock.
  if (lock_fd >= 0) {
    close(lock_fd);
  }
  if (tmp_fd >= 0) {
    if (result < 0) {
<<<<<<< HEAD
      remove(tmp_path);
=======
      remove(tmp_path.c_str());
>>>>>>> origin/ci-clean
    }
    close(tmp_fd);
  }
  return result;
}

<<<<<<< HEAD
int delete_db_value(const char* key, bool persistent_param) {
  int lock_fd = -1;
  int result;
  char path[1024];
  const char* params_path = persistent_param ? persistent_params_path : default_params_path;

  // Build lock path, and open lockfile
  result = snprintf(path, sizeof(path), "%s/.lock", params_path);
  if (result < 0) {
    goto cleanup;
  }
  lock_fd = open(path, O_CREAT);
=======
int Params::delete_db_value(std::string key) {
  int lock_fd = -1;
  int result;
  std::string path;

  // Build lock path, and open lockfile
  path = params_path + "/.lock";
  lock_fd = open(path.c_str(), O_CREAT, 0775);
>>>>>>> origin/ci-clean

  // Take lock.
  result = flock(lock_fd, LOCK_EX);
  if (result < 0) {
    goto cleanup;
  }

<<<<<<< HEAD
  // Build key path
  result = snprintf(path, sizeof(path), "%s/d/%s", params_path, key);
  if (result < 0) {
    goto cleanup;
  }

  // Delete value.
  result = remove(path);
=======
  // Delete value.
  path = params_path + "/d/" + key;
  result = remove(path.c_str());
>>>>>>> origin/ci-clean
  if (result != 0) {
    result = ERR_NO_VALUE;
    goto cleanup;
  }

  // fsync parent directory
<<<<<<< HEAD
  result = snprintf(path, sizeof(path), "%s/d", params_path);
  if (result < 0) {
    goto cleanup;
  }

  result = fsync_dir(path);
=======
  path = params_path + "/d";
  result = fsync_dir(path.c_str());
>>>>>>> origin/ci-clean
  if (result < 0) {
    goto cleanup;
  }

cleanup:
  // Release lock.
  if (lock_fd >= 0) {
    close(lock_fd);
  }
  return result;
}

<<<<<<< HEAD
int read_db_value(const char* key, char** value, size_t* value_sz, bool persistent_param) {
  char path[1024];
  const char* params_path = persistent_param ? persistent_params_path : default_params_path;

  int result = snprintf(path, sizeof(path), "%s/d/%s", params_path, key);
  if (result < 0) {
    return result;
  }

  *value = static_cast<char*>(read_file(path, value_sz));
=======
std::string Params::get(std::string key, bool block){
  char* value;
  size_t size;
  int r;

  if (block){
    r = read_db_value_blocking((const char*)key.c_str(), &value, &size);
  } else {
    r = read_db_value((const char*)key.c_str(), &value, &size);
  }

  if (r == 0){
    std::string s(value, size);
    free(value);
    return s;
  } else {
    return "";
  }
}

int Params::read_db_value(const char* key, char** value, size_t* value_sz) {
  std::string path = params_path + "/d/" + std::string(key);
  *value = static_cast<char*>(read_file(path.c_str(), value_sz));
>>>>>>> origin/ci-clean
  if (*value == NULL) {
    return -22;
  }
  return 0;
}

<<<<<<< HEAD
void read_db_value_blocking(const char* key, char** value, size_t* value_sz, bool persistent_param) {
  while (1) {
    const int result = read_db_value(key, value, value_sz, persistent_param);
    if (result == 0) {
      return;
    } else {
      // Sleep for 0.1 seconds.
      usleep(100000);
    }
  }
}

int read_db_all(std::map<std::string, std::string> *params, bool persistent_param) {
  int err = 0;
  const char* params_path = persistent_param ? persistent_params_path : default_params_path;

  std::string lock_path = util::string_format("%s/.lock", params_path);
=======
int Params::read_db_value_blocking(const char* key, char** value, size_t* value_sz) {
  params_do_exit = 0;
  void (*prev_handler_sigint)(int) = std::signal(SIGINT, params_sig_handler);
  void (*prev_handler_sigterm)(int) = std::signal(SIGTERM, params_sig_handler);

  while (!params_do_exit) {
    const int result = read_db_value(key, value, value_sz);
    if (result == 0) {
      break;
    } else {
      usleep(100000); // 0.1 s
    }
  }

  std::signal(SIGINT, prev_handler_sigint);
  std::signal(SIGTERM, prev_handler_sigterm);
  return params_do_exit; // Return 0 if we had no interrupt
}

int Params::read_db_all(std::map<std::string, std::string> *params) {
  int err = 0;

  std::string lock_path = params_path + "/.lock";
>>>>>>> origin/ci-clean

  int lock_fd = open(lock_path.c_str(), 0);
  if (lock_fd < 0) return -1;

  err = flock(lock_fd, LOCK_SH);
  if (err < 0) {
    close(lock_fd);
    return err;
  }

<<<<<<< HEAD
  std::string key_path = util::string_format("%s/d", params_path);
=======
  std::string key_path = params_path + "/d";
>>>>>>> origin/ci-clean
  DIR *d = opendir(key_path.c_str());
  if (!d) {
    close(lock_fd);
    return -1;
  }

  struct dirent *de = NULL;
  while ((de = readdir(d))) {
    if (!isalnum(de->d_name[0])) continue;
    std::string key = std::string(de->d_name);
<<<<<<< HEAD
    std::string value = util::read_file(util::string_format("%s/%s", key_path.c_str(), key.c_str()));
=======
    std::string value = util::read_file(key_path + "/" + key);
>>>>>>> origin/ci-clean

    (*params)[key] = value;
  }

  closedir(d);

  close(lock_fd);
  return 0;
}

<<<<<<< HEAD
std::vector<char> read_db_bytes(const char* param_name, bool persistent_param) {
  std::vector<char> bytes;
  char* value;
  size_t sz;
  int result = read_db_value(param_name, &value, &sz, persistent_param);
=======
std::vector<char> Params::read_db_bytes(const char* param_name) {
  std::vector<char> bytes;
  char* value;
  size_t sz;
  int result = read_db_value(param_name, &value, &sz);
>>>>>>> origin/ci-clean
  if (result == 0) {
    bytes.assign(value, value+sz);
    free(value);
  }
  return bytes;
}

<<<<<<< HEAD
bool read_db_bool(const char* param_name, bool persistent_param) {
  std::vector<char> bytes = read_db_bytes(param_name, persistent_param);
=======
bool Params::read_db_bool(const char* param_name) {
  std::vector<char> bytes = read_db_bytes(param_name);
>>>>>>> origin/ci-clean
  return bytes.size() > 0 and bytes[0] == '1';
}
