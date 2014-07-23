package model;
import java.util.Date;

public class Measurement 
{
	private String trajectId;
	private Date timestamp;
	private int velocity;
	private int travelTime;
	
	public String getTrajectId() 
	{
		return trajectId;
	}
	
	public void setTrajectId(String trajectId) 
	{
		this.trajectId = trajectId;
	}
	
	public Date getTimestamp() 
	{
		return timestamp;
	}
	
	public void setTimestamp(Date timestamp) 
	{
		this.timestamp = timestamp;
	}
	
	public int getVelocity() 
	{
		return velocity;
	}
	
	public void setVelocity(int velocity) 
	{
		this.velocity = velocity;
	}
	
	public int getTravelTime() 
	{
		return travelTime;
	}
	
	public void setTravelTime(int travelTime) 
	{
		this.travelTime = travelTime;
	}

	@Override
	public String toString() 
	{
		return "Measurement [trajectId=" + trajectId + ", timestamp="
				+ timestamp + ", velocity=" + velocity + ", travelTime="
				+ travelTime + "]";
	}
	
	
}