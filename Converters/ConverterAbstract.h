
class ConverterAbstract
{
	protected:
		int _fdSerial;

	public:
		ConverterAbstract(int fdSerial);
		virtual void setVoltage(int voltage) = 0;
		virtual void on() = 0;
		virtual void off() = 0;
};
