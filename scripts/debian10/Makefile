include ../../DEPENDENCIES
export
PEPENGINE_VERSION=${pEpEngine}
CURRENT_DISTRO=$(shell basename $(shell pwd))
IMAGE_NAME=${DOCKER_REGISTRY_HOST}/pep-$(CURRENT_DISTRO)-libpepadapter
DOCKERFILE=libpEpAdapter.$(CURRENT_DISTRO).Dockerfile
IS_TAGGED=${TAGGED_BUILD}
ifeq ($(IS_TAGGED), true)
# $CI_COMMIT_TAG is a predefined environment variable from Gitlab
	LIBPEPADAPTER_VERSION=${CI_COMMIT_TAG}
else
	LIBPEPADAPTER_VERSION=$(shell git rev-parse --short=8 HEAD)
endif
all:
	-docker pull $(IMAGE_NAME):latest
	cd ../../ && docker build --build-arg CURRENT_DISTRO=$(CURRENT_DISTRO) \
		      --build-arg DOCKER_REGISTRY_HOST=${DOCKER_REGISTRY_HOST} \
		      --build-arg PEPENGINE_VERSION=$(PEPENGINE_VERSION) \
		      --build-arg LIBPEPADAPTER_VERSION=$(LIBPEPADAPTER_VERSION) \
		      --cache-from $(IMAGE_NAME):latest \
		      --tag=$(IMAGE_NAME):$(LIBPEPADAPTER_VERSION) \
		      --tag=$(IMAGE_NAME):${LIBPEPADAPTER_VERSION}_engine-${PEPENGINE_VERSION} \
		      --tag=$(IMAGE_NAME):latest \
		      -f scripts/${CURRENT_DISTRO}/$(DOCKERFILE) .
	docker push $(IMAGE_NAME):${LIBPEPADAPTER_VERSION}
	docker push $(IMAGE_NAME):${LIBPEPADAPTER_VERSION}_engine-${PEPENGINE_VERSION}
	docker push $(IMAGE_NAME):latest
