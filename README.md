# Shmoocon2016
Materials from the presentation

* See PDF for presentation


## Hunchback and parse

Both files look for a magic number and will then load a payload in memory.

hunchback checks itself for the magic number, executes the payload, then truncates itself (and will pass strict signing checks).

parse looks at another file and will load the payload in memory.

### Payload layout

```[---4 bytes magic number---][---4 bytes magic number ---][--2 bytes payload size--][-----payload-----]```

Use append.py for an example of appending data to a file to match this format.

# Lipocram.py

https://github.com/secretsquirrel/LipoCram

# Kyphosis.py

https://github.com/secretsquirrel/kyphosis
