using UnityEngine;

public class movement : MonoBehaviour
{
    public float speedX = 5f;
    public float speedY = 5f;
    public float speedZ = -1f;


    void Start()
    {
        
    }

    void Update()
    {
        transform.position += new Vector3(
            speedX * Time.deltaTime,
            speedY * Time.deltaTime,
            speedZ * Time.deltaTime
            );
    }
}
