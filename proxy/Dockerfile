FROM ubuntu:jammy

RUN apt-get -y update && apt-get -y install meson g++ && apt-get clean
COPY . /app
WORKDIR /app
RUN meson setup builddir && cd builddir && meson compile

CMD ["./builddir/proxy"]