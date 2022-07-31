# Short URL

Short URL is a quick and dirty tinyurl clone.

* Shortened URL creation
* Redirection and usage tracking
* Retrieval of original URL

<p align="center">
<img src="../assets/short_url_screenshot.png"
  alt="Screen shot of short-url in action"
  width="700" height="500">
</p>

## How It Works

1. Uses Poco for boiler plate web server code
2. Encodes URL to create a shortened URI it following [encoding link]
3. Stores encoded url in a hash with usage count and original link

[encoding link]: https://stackoverflow.com/a/742047

## Build Project

<details><summary><b>Show instructions</b></summary>

1. Clone project:

    ```sh
    $ git clone https://github.com/TrevinoWes/short-url.git
    ```
2. Clone submodules

    ```sh
    $ git submodule update --init --recursive

    ```

3. Create build directory

    ```sh
    $ mkdir build && cd build
    ```

4. Using CMake to create Makefile

    ```sh
    $ cmake ..
    ```

5. Call Make to create Binary

    ```sh
    $ make
    ```

</details>

## Potential Improvements

* Create full object to manage database or memory used to save URL info
* Implement a token that allows validation of our frontend (Will make it harder to ddos)
* Use a thread pool for multi threading the server (shared objects are already thread safe)
* Config file to set socket and other server parameters
* Add logging
