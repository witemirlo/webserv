#ifndef _HTTP_STATUS_CODE_HPP
# define _HTTP_STATUS_CODE_HPP

# define CONTINUE                      100
# define SWITCHING_PROTOCOLS           101
# define OK                            200
# define CREATED                       201
# define ACCEPTED                      202
# define NON_AUTHORITATIVE_INFORMATION 203
# define NO_CONTENT                    204
# define RESET_CONTENT                 205
# define PARTIAL_CONTENT               206
# define MULTIPLE_CHOICES              300
# define MOVED_PERMANENTLY             301
# define FOUND                         302
# define SEE_OTHER                     303
# define NOT_MODIFIED                  304
# define USE_PROXY                     305
# define TEMPORARY_REDIRECT            307
# define PERMANENT_REDIRECT            308
# define BAD_REQUEST                   400
# define UNAUTHORIZED                  401
# define PAYMENT_REQUIRED              402
# define FORBIDDEN                     403
# define NOT_FOUND                     404
# define METHOD_NOT_ALLOWED            405
# define NOT_ACCEPTABLE                406
# define PROXY_AUTHENTICATION_REQUIRED 407
# define REQUEST_TIMEOUT               408
# define CONFLICT                      409
# define GONE                          410
# define LENGTH_REQUIRED               411
# define PRECONDITION_FAILED           412
# define CONTENT_TOO_LARGE             413
# define URI_TOO_LONG                  414
# define UNSUPPORTED_MEDIA_TYPE        415
# define RANGE_NOT_SATISFIABLE         416
# define EXPECTATION_FAILED            417
# define MISDIRECTED_REQUEST           421
# define UNPROCESSABLE_CONTENT         422
# define UPGRADE_REQUIRED              426
# define INTERNAL_SERVER_ERROR         500
# define NOT_IMPLEMENTED               501
# define BAD_GATEWAY                   502
# define SERVICE_UNAVAILABLE           503
# define GATEWAY_TIMEOUT               504
# define HTTP_VERSION_NOT_SUPPORTED    505

#endif // !_HTTP_STATUS_CODE_HPP
