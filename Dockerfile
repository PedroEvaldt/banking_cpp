FROM ubuntu:22.04

# Evita prompts interativos
ENV DEBIAN_FRONTEND=noninteractive

# Instala dependências de compilação e PostgreSQL
RUN apt update && apt install -y \
    g++ make cmake git libpq-dev libssl-dev libboost-all-dev

# ==============================
# Compila e instala a biblioteca SOCI manualmente
# ==============================
RUN git clone --branch v4.0.3 --recurse-submodules https://github.com/SOCI/soci.git /tmp/soci && \
    cd /tmp/soci && mkdir build && cd build && \
    cmake -DWITH_BOOST=ON -DWITH_POSTGRESQL=ON -DWITH_CXX11=ON -DCMAKE_CXX_STANDARD=11 .. && \
    make -j$(nproc) && make install && \
    rm -rf /tmp/soci

# ==============================
# Define diretório de trabalho
# ==============================
WORKDIR /app

# Copia todo o código do projeto
COPY . .

# Compila o projeto
RUN mkdir build && cd build && cmake .. && make -j$(nproc)

# Executa o programa
CMD ["./bin/bank_app"]
