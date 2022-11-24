FROM ubuntu:jammy

ARG GIT_CHECKOUT

WORKDIR /root

RUN apt-get update \
  && apt-get install -y git gcc g++ libpq-dev libpq5 make automake libtool pkg-config libaio-dev vim-common postgresql-client \
  && cd /root \
  && git clone https://github.com/akopytov/sysbench.git \
  && cd /root/sysbench \
  && git checkout $GIT_CHECKOUT \
  && ./autogen.sh \
  && ./configure --without-mysql --with-pgsql \
  && make \
  && make install \
  && make clean

COPY "run_sysbench.sh" /root
COPY "intro_message.sh" /root
RUN chmod +x intro_message.sh
RUN chmod +x run_sysbench.sh
ENTRYPOINT ["./intro_message.sh"]