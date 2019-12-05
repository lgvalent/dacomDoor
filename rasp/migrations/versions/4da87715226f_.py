"""empty message

Revision ID: 4da87715226f
Revises: 
Create Date: 2019-03-13 09:06:02.478562

"""
from alembic import op
import sqlalchemy as sa


# revision identifiers, used by Alembic.
revision = '4da87715226f'
down_revision = None
branch_labels = None
depends_on = None


def upgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    op.create_table('events',
    sa.Column('id', sa.Integer(), nullable=False),
    sa.Column('uid', sa.String(length=16), nullable=False),
    sa.Column('eventType', sa.Enum('IN', 'OUT', name='eventtypesenum'), nullable=False),
    sa.Column('dateTime', sa.DateTime(), nullable=False),
    sa.PrimaryKeyConstraint('id')
    )
    op.create_table('keyrings',
    sa.Column('id', sa.Integer(), nullable=False),
    sa.Column('uid', sa.String(length=16), nullable=False),
    sa.Column('userId', sa.Integer(), nullable=False),
    sa.Column('userType', sa.Enum('STUDENT', 'PROFESSOR', 'EMPLOYEE', name='usertypesenum'), nullable=False),
    sa.Column('lastUpdate', sa.DateTime(), nullable=False),
    sa.PrimaryKeyConstraint('id'),
    sa.UniqueConstraint('uid')
    )
    op.create_table('schedules',
    sa.Column('id', sa.Integer(), nullable=False),
    sa.Column('dayOfWeek', sa.Enum('MONDAY', 'THUESDAY', 'WEDNESDAY', 'THURSDAY', 'FRIDAY', 'SATURDAY', 'SUNDAY', name='daysofweekenum'), nullable=False),
    sa.Column('beginTime', sa.Time(), nullable=False),
    sa.Column('endTime', sa.Time(), nullable=False),
    sa.Column('userType', sa.Enum('STUDENT', 'PROFESSOR', 'EMPLOYEE', name='usertypesenum'), nullable=False),
    sa.Column('lastUpdate', sa.DateTime(), nullable=False),
    sa.PrimaryKeyConstraint('id')
    )
    # ### end Alembic commands ###


def downgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    op.drop_table('schedules')
    op.drop_table('keyrings')
    op.drop_table('events')
    # ### end Alembic commands ###