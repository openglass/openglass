BOOST_VERSION=boost_1_49_0

INCLUDE_BOOST=-I/appl/third_party/$(BOOST_VERSION)
LIBS_BOOST=-L/appl/third_party/$(BOOST_VERSION)/lib

THIRD_PARTY_INCLUDE=$(INCLUDE_BOOST)
THIRD_PARTY_LIBS=$(LIBS_BOOST)

#INCLUDE_MYSQL_CONN=-I/appl/third_party/mysql-connector-c++-1.0.5/cppconn
#LIBS_MYSQL_CONN=-L/appl/third_party/mysql-connector-c++-1.0.5/lib

INCLUDE_PQXX=-I/appl/third_party/pgsql-9.1/include -I/appl/third_party/pqxx-4.0/include
#LIBS_PQXX= -L/appl/third_party/pqxx-4.0/lib -lpqxx -L/usr/pgsql-9.1/lib -lpq.a
LIBS_PQXX= /appl/third_party/pqxx-4.0/lib/libpqxx.a /appl/third_party/pgsql-9.1/lib/libpq.a

THIRD_PARTY_INCLUDE+=$(INCLUDE_PQXX)
THIRD_PARTY_LIBS+=$(LIBS_PQXX)

