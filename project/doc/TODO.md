# TODO

## Next Update

> [!NOTE]
> crun cli --> c
> crun gui --> c++

### PROBLEM

- **DEV PROBLEM**: To add new project we need to modify _crun bin_
- **USER PROBLEM**: User need to have internet to download project

### SOLUTION

- We will work on the next formula: `crun = crun core + stacks`
- `Crun core` will load available `stacks` (on local)
- We will keep the download option as `--download` option
- We will add `--update` to update the local stacks

> stacks = projects + scripts + ...

### HOW?

- We have `usr/local/bin/crun` ==> **crun core** :)
- We will add `usr/local/share/crun_stacks/*` ==> stacks + crun_stacks.json

> crun_stacks.json will have stacks structure + latest commit (for next update)

### DEV

#### `./crun`

1. `crun` will see `crun_stacks.json` and fetch the available stacks using `nlohmann/json` (local)
2. chose the stack
3. copy the stack into the current user path location
4. Other precess..

#### `./crun --download <stack_name>`

1. `crun` will see `cpkg` repo and see the project name
2. download it locally
3. Other precess

### `./crun --update`

1. `crun` will see the local `crun_stacks.json` and compare it with `crun/doc/crun_stacks.json` using `sha256sum`
2. If there is a difference, we fetch the remote `crun_stacks.json` and we replace it with the local one
   \*. We can make partial update

> If `crun_stacks.json` not exist, we will fetch `crun/doc/crun_stacks.json` under `usr/local/share/` and complete the update

### References

- [cpp-json](https://github.com/nlohmann/json)
- [C++ sha256 function](http://www.zedwood.com/article/cpp-sha256-function)
- [SHA256](https://www.programmingalgorithms.com/algorithm/sha256/cpp/)
- [Generate sha256 with OpenSSL and C++](https://stackoverflow.com/questions/2262386/generate-sha256-with-openssl-and-c/10632725)
- [cJSON](https://github.com/DaveGamble/cJSON)
