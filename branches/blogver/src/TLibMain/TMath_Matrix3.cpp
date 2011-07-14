#include "TMath_Matrix3.h"

namespace TsiU
{
	const Mat3 Mat3::ZERO(0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f);
	const Mat3 Mat3::IDENTITY(1.f,0.f,0.f,0.f,1.f,0.f,0.f,0.f,1.f);
	
	Bool Mat3::operator == (const Mat3& m)
	{
		for(u32 row = 0; row < 3; ++row)
		{
			for(u32 column = 0; column < 3; ++column)
			{
				if(Math::IsEqual(mat[row][column], m.mat[row][column]) == false)
					return false;
			}
		}
		return true;
	}

	Mat3 Mat3::operator + (const Mat3& m)
	{
		Mat3 resultM;
		for(u32 row = 0; row < 3; ++row)
		{
			for(u32 column = 0; column < 3; ++column)
			{
				resultM[row][column] = mat[row][column] + m.mat[row][column];
			}
		}
		return resultM;
	}

	Mat3 Mat3::operator - (const Mat3& m)
	{
		Mat3 resultM;
		for(u32 row = 0; row < 3; ++row)
		{
			for(u32 column = 0; column < 3; ++column)
			{
				resultM[row][column] = mat[row][column] - m.mat[row][column];
			}
		}
		return resultM;
	}

	Mat3 Mat3::operator - ()
	{
		Mat3 resultM;
		for(u32 row = 0; row < 3; ++row)
		{
			for(u32 column = 0; column < 3; ++column)
			{
				resultM[row][column] = -mat[row][column];
			}
		}
		return resultM;
	}

	Mat3 Mat3::operator * (const Mat3& m)
	{
		Mat3 resultM;
		for(u32 row = 0; row < 3; ++row)
		{
			for(u32 column = 0; column < 3; ++column)
			{
				resultM[row][column] = mat[row][0] * m.mat[0][column] + 
										   mat[row][1] * m.mat[1][column] +
										   mat[row][2] * m.mat[2][column];
			}
		}
		return resultM;
	}

	Vec3 Mat3::operator * (const Vec3& v)
	{
		Vec3 resultV;
		for(u32 row = 0; row < 3; ++row)
		{
			resultV[row] =	mat[row][0] * v[0] +
							mat[row][1] * v[1] +
							mat[row][2] * v[2];
		}
		return resultV;
	}

	Mat3 Mat3::operator * (const f32 fv)
	{
		Mat3 resultM;
		for(u32 row = 0; row < 3; ++row)
		{
			for(u32 column = 0; column < 3; ++column)
			{
				resultM[row][column] = mat[row][0] * fv;
			}
		}
		return resultM;
	}
	Mat3 operator * (const f32 fv, const Mat3& m)
	{
		Mat3 resultM;
		for(u32 row = 0; row < 3; ++row)
		{
			for(u32 column = 0; column < 3; ++column)
			{
				resultM[row][column] = fv * m.mat[row][column];
			}
		}
		return resultM;
	}
	Vec3 operator * (const Vec3& v, const Mat3& m)
	{
		Vec3 resultV;
		for(u32 column = 0; column < 3; ++column)
		{
			resultV[column] = m.mat[0][column] * v[0] +
							  m.mat[1][column] * v[1] +
							  m.mat[2][column] * v[2];
		}
		return resultV;
	}
	Mat3 Mat3::Transpose()
	{
		Mat3 resultM;
		for(u32 row = 0; row < 3; ++row)
		{
			for(u32 column = 0; column< 3; ++column)
			{
				resultM[column][row] = mat[row][column];
			}
		}
		return resultM;
	}
	Mat3 Mat3::Inverse()
	{
		Mat3 resultM;
		resultM = *this;
		u32 is[3];
		u32 js[3];

		for(s32 k = 0; k < 3; ++k)
		{
			f32 fMax = 0.0f;
			for(u32 i = k; i < 3; i ++)
			{
				for (u32 j = k; j < 3; j ++)
				{
					const f32 f = Math::Abs<f32>(resultM[i][j]);
					if(f > fMax)
					{
						fMax = f;
						is[k] = i;
						js[k] = j;
					}
				}
			}
			if(Math::IsZero(fMax))
				return ZERO;

			if(is[k] != k)
			{
				for(u32 i = 0; i < 3; ++i)
				{
					f32 tmp = resultM[k][i];
					resultM[k][i] = resultM[is[k]][i];
					resultM[is[k]][i] = tmp;
				}
			}
			if (js[k] != k)
			{
				for(u32 i = 0; i < 3; ++i)
				{
					f32 tmp = resultM[i][k];
					resultM[i][k] = resultM[i][js[k]];
					resultM[i][js[k]] = tmp;
				}
			}
			resultM[k][k] = 1.0f / resultM[k][k];
			for(u32 j = 0; j < 3; j ++)
			{
				if (j != k)
					resultM[k][j] *= resultM[k][k];
			}
			for(u32 i = 0; i < 3; i ++)
			{
				if (i != k)
				{
					for (u32 j = 0; j < 3; j ++)
					{
						if (j != k)
							resultM[i][j] = resultM[i][j] - resultM[i][k] * resultM[k][j];
					}
				}
			}
			for(u32 i = 0; i < 3; i ++)
			{
				if (i != k)
					resultM[i][k] *= -resultM[k][k];
			}
		}

		for(u32 k = 2; k >= 0; k --)
		{
			if (js[k] != k)
			{
				for(u32 i = 0; i < 3; ++i)
				{
					f32 tmp = resultM[k][i];
					resultM[k][i] = resultM[js[k]][i];
					resultM[js[k]][i] = tmp;
				}
			}
			if (is[k] != k)
			{
				for(u32 i = 0; i < 3; ++i)
				{
					f32 tmp = resultM[i][k];
					resultM[i][k] = resultM[i][is[k]];
					resultM[i][is[k]] = tmp;
				}
			}
		}
		return resultM;
	}
}