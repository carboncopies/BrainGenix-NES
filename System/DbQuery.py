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
        cursor.execute("select n.Neurons_id, n.brainregion, sc.Synapses_id, sc.ConnectedToSynapses_id, sc.Strength " +
                   "from bgdb.Neurons n inner join bgdb.Synapses s on s.Neurons_id = n.Neurons_id " +
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
        cursor.execute("select user_name, aes_decrypt(%s, %s) as password, first_name, last_name as password from bgdb.User where User_Name = %s;", (userPass, userName, userName))
        rows = cursor.fetchall()
        result = "success"

        #print('Total Row(s):', cursor.rowcount)
        for row in rows:
            print(row)

    except Error as e:
        result = e

    finally:
        cursor.close()
        conn.close()
        return result

if __name__ == '__main__':
    UserLogin()

def UserAdd(userName, userPass, userFirst, userLast):

    result = ""

    try:
        dbconfig = read_db_config()
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
        cursor.execute("insert into bgdb.User (user_name, user_password, first_name, last_name) values (%s, aes_encrypt(%s, %s), %s, %s);", (userName, userPass, userName, userFirst, userLast))
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