#!/bin/bash

# Check if the version argument is provided
if [ -z "$1" ]; then
    read -p "Please enter the name of the docker file file: " filename  # Prompt for input
else
    filename="$1"  # Use the provided value
fi

docker build -t ns-3-corso-img -f $filename .
# docker stop ns-3-corso # in case you need to stop the container
# docker rm ns-3-corso # in case you need to delete the image
# If you want to create a folder that is connected in your workspace, add the following flag -v <path_on_host>/<path_on_container>
docker run -d -it --name ns-3-con ns-3-img

echo "Creation of teh image and run of the container done, get inside with: docker exec -it ns-3-con bash"
