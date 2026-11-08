FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    clang-18 \
    python3 \
    python3-pip \
    git \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Set Clang 18 as default
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100

WORKDIR /workspace
COPY . /workspace

# Build C++ Static Verifier
RUN mkdir -p build && cd build && \
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. && \
    ninja

# Default command: run empirical benchmark suite
CMD ["python3", "benchmarks/run_benchmark.py"]
