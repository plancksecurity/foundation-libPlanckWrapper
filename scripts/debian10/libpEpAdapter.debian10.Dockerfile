ARG DOCKER_REGISTRY_HOST
ARG CURRENT_DISTRO
ARG PEPENGINE_VERSION=latest
FROM ${DOCKER_REGISTRY_HOST}/pep-${CURRENT_DISTRO}-engine:${PEPENGINE_VERSION}

ENV BUILDROOT /build
ENV INSTPREFIX /install
ENV OUTDIR /out

### Setup working directory
RUN mkdir ${BUILDROOT}/libpEpAdapter
COPY . ${BUILDROOT}/libpEpAdapter
USER root
RUN chown -R pep-builder:pep-builder ${BUILDROOT}/libpEpAdapter
USER pep-builder
WORKDIR ${BUILDROOT}/libpEpAdapter

ARG LIBPEPADAPTER_VERSION
ARG CURRENT_DISTRO

### Build libpEpAdapter
RUN sh ./scripts/${CURRENT_DISTRO}/build_libpEpAdapter.sh && \
    rm -rf ${BUILDROOT}/*
