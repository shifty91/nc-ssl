# NC-SSL #

Ever wanted to use nc for connections over SSL (like HTTPS)? Well it won't work.
You need to use different tools such as openssl sclient or socat. To me these
tools have strange cmd line options, so I ended up with a really simple netcat
utility which speaks SSL/TLS to the server.

# Usage #

    $ usage: nc-ssl [options] <host> <service>
    $   options:
    $    -d: enable debug output
    $    -v: verify hosts's certificate
    $    -2: use SSL version 2
    $    -3: use SSL version 3

# Example #

    $ nc-ssl www.google.de https
    GET / HTTP/1.1
    Host: www.google.de
    Connection: Close

    HTTP/1.1 200 OK
    Date: Sat, 19 Sep 2015 12:28:11 GMT
    Expires: -1
    Cache-Control: private, max-age=0
    Content-Type: text/html; charset=ISO-8859-1
    P3P: CP="This is not a P3P policy! See http://www.google.com/support/accounts/bin/answer.py?hl=en&answer=151657 for more info."
    Server: gws
    X-XSS-Protection: 1; mode=block
    X-Frame-Options: SAMEORIGIN
    Set-Cookie: PREF=ID=1111111111111111:FF=0:TM=1442665691:LM=1442665691:V=1:S=WJOx01_PTcChKOAz; expires=Thu, 31-Dec-2015 16:02:17 GMT; path=/; domain=.google.de
    Set-Cookie: NID=71=VnOjA99_uC_vflX-cy_AfRPbNko97nNeEyPBZNdAZD1kU62bi8hUjagI4lEoKSKleAuH1M1zIg24PSbG-Vhnsf7dCNNMxgbKifcehnb8CA-E3UKLatupYQysZQB0YIBfI6PnVK8; expires=Sun, 20-Mar-2016 12:28:11 GMT; path=/; domain=.google.de; HttpOnly
    Accept-Ranges: none
    Vary: Accept-Encoding
    Connection: close

    [ ... ]

# Build #

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make -j8
    $ (sudo make install)

# Dependencies #

- OpenSSL

# Author #

(C) Kurt Kanzenbach 2015-2016,2021 <kurt@kmk-computers.de>

# License #

GPL v2+
