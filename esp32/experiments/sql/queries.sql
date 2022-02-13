-- Check if rfid is allowed
SELECT * 
FROM rfids_permitidos
WHERE rfid = '{rfid}'
LIMIT 1;


SELECT * FROM horarios_permitidos
WHERE (
  tipo_usuario = '{tipo_usuario}' and
  dia = '{dia}' and 
  hora_inicio <= TIME('{hora}') and 
  hora_fim >= TIME('{hora}')
)
LIMIT 1;


INSERT INTO eventos(rfid, tipo, horario) VALUES 
('{rfid}', '{tipo}', '{horario}');