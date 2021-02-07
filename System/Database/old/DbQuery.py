import mysql.connector
from mysql.connector import Error

from mysql.connector import MySQLConnection, Error
from DbMysqlConfig import read_db_config


#https://www.mysqltutorial.org/python-mysql-query/
def GetNeuronInfoByNeuronId(neuronId):

    try:
        dbconfig = read_db_config()
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
        cursor.execute("select n.neuronId, n.xCoord, sc.equationId, sc.ConnectedToSynapses_id, sc.Strength " +
                   "from bgdb.neuron n inner join bgdb.synapse s on s.neuronsId = n.neuronsId " +
                   "inner join bgdb.SynapseConnections sc on sc.synapses_id = s.synapses_id " +
                   "where n.Neurons_id = %s", (neuronId))
        rows = cursor.fetchall()

        print('Total Row(s):', cursor.rowcount)
        for row in rows:
            print(row)

    except Error as e:
        print(e)

    finally:
        cursor.close()
        conn.close()


if __name__ == '__main__':
    GetNeuronInfoByNeuronId()

def UserLogin(userName, userPass):

    result = ""

    try:
        dbconfig = read_db_config()
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
        cursor.execute("select userName, aes_decrypt(%s, %s) as password, firstName, lastName as password from bgdb.user where UserName = %s;", (userPass, userName, userName))
        rows = cursor.fetchall()
        result = "success"

        #print('Total Row(s):', cursor.rowcount)
        for row in rows:
            print(row)

    except Error as e:
        print(e)
        result = e

    finally:
        cursor.close()
        conn.close()
        return result

if __name__ == '__main__':
    UserLogin()

def UserAdd(userId, userName, userPass, userSalt, userFirst, userLast):

    result = ""

    try:
        dbconfig = read_db_config()
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
        cursor.execute("insert into bgdb.user (userId, userName, passwordHash, salt, firstName, lastName, notes, accountEnabled, accountExpirationDate, passwordExpirationDate) values (%s, %s, %s, %s, %s, %s, '', 'T', null, null );",
                       (userId, userName, userPass, userSalt, userFirst, userLast))
        conn.commit()
        result = "success"

    except Error as e:
        result = e

    finally:
        cursor.close()
        conn.close()
        print(result)

if __name__ == '__main__':
    UserAdd()