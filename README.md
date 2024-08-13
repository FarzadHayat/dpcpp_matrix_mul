# Download Pages

- [Intel® oneAPI Base Toolkit](https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html)
- [Intel® HPC Toolkit](https://www.intel.com/content/www/us/en/developer/tools/oneapi/hpc-toolkit-download.html)

# Installation Steps

To make these instructions as universal as possible, we will setup an x86 Ubuntu docker container from which to run the Intel DPC++ Compiler.

0. [Install Docker and start the service](https://www.docker.com/) (I'm on Apple Silicon hardware, so I'm using [Colima](https://formulae.brew.sh/formula/colima) as my Docker runtime along with [docker cli](https://formulae.brew.sh/formula/docker) to avoid the need for Docker Desktop).

1. Start Ubuntu docker container:

```bash
docker run -it --platform linux/amd64 --cpus=8 ubuntu bash
```

2. Install some required packages:

```bash
apt update
apt install -y gpg-agent wget git cmake pkg-config build-essential
```

3. Import the Intel repository:

```bash
wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB | gpg --dearmor | tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null
echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | tee /etc/apt/sources.list.d/oneAPI.list
```

4. Install the Intel oneAPI Base Toolkit and Intel HPC Toolkit (be warned, this will take a few minutes, these packages are huge):

```bash
apt update
apt install -y intel-basekit intel-hpckit
```

5. Source the Intel oneAPI environment:

```bash
source /opt/intel/oneapi/setvars.sh
```

6. Clone this repository and build the code (this will take a few seconds):

```bash
cd /home/ubuntu
git clone https://github.com/FarzadHayat/dpcpp_matrix_mul.git
cd dpcpp_matrix_mul
make
```

7. Run the code:

```bash
make run
```

Example output (your times will vary):

```bash
Device: QEMU TCG CPU version 2.5+
Number of compute units: 8
       Serial time: 4.39493s
     Parallel time: 0.695102s
Both versions produced correct results.
```

As you can see, the parallel version is about 6.3x faster than the serial version in this case.

8. Use the oneAPI CLI to run more samples (check the README.md in each sample directory for build and run instructions):

```bash
oneapi-cli
```

9. When you're done, exit the container:

```bash
exit
```

---

Good luck, and have fun! 🚀