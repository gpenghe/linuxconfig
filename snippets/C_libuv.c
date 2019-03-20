#include <stdlib.h>
#include <string.h>

#include <uv.h>

void on_close(uv_handle_t* handle) {
    free(handle);
}

void on_read(uv_stream_t* tcp_handle, long int nread, const uv_buf_t* buf) {
    // Error or end of file
    if (nread == UV_EOF) {
        // No more data. Close the connection
        uv_close((uv_handle_t*)tcp_handle, on_close);
    } else if (nread < 0) {
        fprintf(stderr, "on_read error: nread:%ld\n", nread);
        abort();
    }

    if (nread >0) {
        /* Print it! */
        fwrite(buf->base, 1, nread, stdout);
    }

    free(buf->base);
}

void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    buf->base = malloc(suggested_size);
    buf->len = suggested_size;
}

void after_write(uv_write_t* req, int status) {
    if (status < 0) {
        fprintf(stderr, "after_write error: status: %d\n", status);
        abort();
    }

    printf("uv_write is successful\n");
}

void after_connect(uv_connect_t* connect_req, int status) {
    uv_write_t* write_req;
    uv_buf_t buf;

    if (status < 0) {
        fprintf(stderr, "after_connect error: status: %d\n", status);
        abort();
    }

    write_req = malloc(sizeof(uv_write_t));

    buf.base =
        "GET / HTTP/1.0\r\n"
        "Host: nyan.cat\r\n"
        "\r\n";

    buf.len = strlen(buf.base);

    uv_write(write_req, connect_req->handle, &buf, 1, after_write);

    uv_read_start(connect_req->handle, on_alloc, on_read);

    free(connect_req);
}

void after_getaddrinfo(uv_getaddrinfo_t* gai_req, int status, struct addrinfo* ai) {
    uv_tcp_t* tcp_handle;
    uv_connect_t* connect_req;

    if (status < 0) {
        fprintf(stderr, "after_getaddrinfo error: status: %d\n", status);
        abort();
    }

    tcp_handle = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), tcp_handle);

    connect_req = malloc(sizeof(uv_connect_t));
    uv_tcp_connect(
            connect_req,
            tcp_handle,
            ai->ai_addr,
            after_connect);

    free(gai_req);
    uv_freeaddrinfo(ai);
}

int main(int argc, char* argv[]) {

    char* server_url = "nyan.cat";

    if (argc >= 2) {
        server_url = argv[1];
    }

    uv_getaddrinfo_t* gai_req = malloc(sizeof(uv_getaddrinfo_t));

    uv_getaddrinfo(
            uv_default_loop(),
            gai_req,
            after_getaddrinfo,
            server_url,
            "80",
            NULL);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    return 0;
}

