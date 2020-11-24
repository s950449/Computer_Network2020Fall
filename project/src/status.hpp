class HTTPSpec{
    public:
        HTTPSpec(){};
        enum class Version{
            HTTP1_0 = 10000,
            HTTP1_1 = 10001,
            HTTP1_2 = 10002
        };
        enum class StatusCode{
            OK = 200,
            CREATED = 201,
            NO_CONTENT = 204,
            NOT_FOUND = 404,
            METHOD_NOT_VALID = 405,
            I_M_A_TEAPOT = 418
        };
        enum class Method{
            GET,POST,INVALID
        };
};


