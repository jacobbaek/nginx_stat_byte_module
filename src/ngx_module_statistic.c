
/* 
 * Copyright (C) doun.baek
 * Copyright (C) Nginx, Inc.
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static u_char ngx_statistic_string[] = "test string";

static ngx_http_output_header_filter_pt ngx_http_next_header_filter;
static ngx_http_output_body_filter_pt ngx_http_next_body_filter;

static char *ngx_http_statistic(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_statistic_filter_init(ngx_conf_t *cf);

/* directives */
static ngx_command_t ngx_http_statistic_commands[] = {
	{ ngx_string("statistic"),					// command name
	  NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,		// level of config
	  ngx_http_statistic,						// pointer of handler-function
	  0,										// not using
	  0,										// not using
	  NULL},									// pointer post-processing handler
	  ngx_null_command
};

static ngx_http_module_t ngx_http_statistic_module_ctx = {
	NULL, 
	ngx_http_statistic_filter_init, /* for filter */

	NULL,
	NULL,

	NULL,
	NULL,

	NULL,
	NULL
};

/* module description structure */
ngx_module_t ngx_http_statistic_module = {
	NGX_MODULE_V1,
	&ngx_http_statistic_module_ctx,
	ngx_http_statistic_commands,
	NGX_HTTP_MODULE,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NGX_MODULE_V1_PADDING
};

static ngx_int_t
ngx_http_statistic_header_filter(ngx_http_request_t *r)
{

	return ngx_http_next_header_filter(r);
}

static ngx_int_t
ngx_http_statistic_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
	ngx_int_t rc;
	ngx_chain_t *out;

	out=NULL;

	rc = ngx_http_next_body_filter(r, out);

	return rc;
}

/* detail module handler */
static ngx_int_t
ngx_http_statistic_handler(ngx_http_request_t *r)
{
	ngx_buf_t    *b;
    ngx_chain_t   out;

	/* header send */
	ngx_http_send_header(r);

	/* create response buffer */ 
	b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
	if (b == NULL) {
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, 
		"Failed to allocate response buffer.");
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	}

	b->pos = ngx_statistic_string; /* first position in memory of the data */
	b->last = ngx_statistic_string + sizeof(ngx_statistic_string) - 1; /* last position */
    b->memory = 1; /* content is in read-only memory */
    b->last_buf = 1; /* there will be no more buffers in the request */

	/* attach this buffer to the buffer chain */
    out.buf = b;
    out.next = NULL;

	/* send body */
	return ngx_http_output_filter(r, &out);
}

/* declare a handler for statistic */
static char *
ngx_http_statistic(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_http_core_loc_conf_t *clcf;
	
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_statistic_handler;

	return NGX_CONF_OK;
}

/* write statistic value in this filter */
static ngx_int_t
ngx_http_statistic_filter_init(ngx_conf_t *cf)
{
	ngx_http_next_header_filter = ngx_http_top_header_filter;
	ngx_http_top_header_filter = ngx_http_statistic_header_filter;

	ngx_http_next_body_filter = ngx_http_top_body_filter;
	ngx_http_top_body_filter = ngx_http_statistic_body_filter;

	return NGX_OK;
}
