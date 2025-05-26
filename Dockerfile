FROM ubuntu:24.04

# AstraKernel Dockerfile
# ======================
# Run the following to start:
# $ make docker

# 1. Install dependencies
RUN apt-get update && apt-get install -y \
    # Install git for cloning
    git \
    # Install bare-metal ARM essentials
    build-essential gcc-arm-none-eabi binutils-arm-none-eabi \
    qemu-system-arm \
    # Clear apt cache
    && rm -rf /var/lib/apt/lists/*

# 2. Clone the most recent version of the main branch
WORKDIR /
RUN git clone --single-branch -b main --depth=1 https://github.com/sandbox-science/AstraKernel.git
WORKDIR /AstraKernel

# 3. Build/Run via QEMU
RUN make kernel.bin

CMD ["make", "qemu"]
