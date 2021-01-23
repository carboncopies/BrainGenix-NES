import mysql.connector
from mysql.connector import Error

from mysql.connector import MySQLConnection, Error
from DbMysqlConfig import read_db_config


#https://www.mysqltutorial.org/python-mysql-query/
def GetNeuronInfoByNeuronId():

    try:
        dbconfig = read_db_config()
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
        cursor.execute("select n.Neurons_id, n.brainregion, sc.Synapses_id, sc.ConnectedToSynapses_id, sc.Strength " +
                   "from bgdb.Neurons n inner join bgdb.Synapses s on s.Neurons_id = n.Neurons_id " +
                   "inner join bgdb.SynapseConnections sc on sc.synapses_id = s.synapses_id " +
                   "where n.Neurons_id = 1")
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

