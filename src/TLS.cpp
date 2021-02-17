//
// Created by kmin on 2021/02/15.
//
# include "Server.hpp"

SSL_CTX	*Server::InitCTX(void)
{
	const SSL_METHOD *method;
	SSL_CTX *ctx;

	OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
	SSL_load_error_strings();   /* load all error messages */
	method = SSLv23_server_method();  /* create new server-method instance */
	ctx = SSL_CTX_new(method);   /* create new context from method */
	if ( ctx == NULL ) {
		ERR_print_errors_fp(stderr);
		std::cerr << "in InitCTX() error" << std::endl;
	}
	return ctx;
}

void Server::LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile, bool serv)
{
	if (serv) {
		//New lines
		if (SSL_CTX_load_verify_locations(ctx, CertFile, KeyFile) != 1)
			ERR_print_errors_fp(stderr);

		if (SSL_CTX_set_default_verify_paths(ctx) != 1)
			ERR_print_errors_fp(stderr);
		//End new lines
	}

	/* set the local certificate from CertFile */
	if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		abort();
	}
	/* set the private key from KeyFile (may be the same as CertFile) */
	if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		abort();
	}
	/* verify private key */
	if (!SSL_CTX_check_private_key(ctx))
	{
		fprintf(stderr, "Private key does not match the public certificate\n");
		abort();
	}

	/*
		//New lines Force the client-side have a certificate
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
		SSL_CTX_set_verify_depth(ctx, 4);
		//End new lines
	*/
}
